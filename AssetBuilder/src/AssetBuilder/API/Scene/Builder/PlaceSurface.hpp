// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::placeSurface(const SurfaceDescriptor& descriptor, const TransformAngleAxis3D& transform) {
		const uint64 surfaceVertexCount = descriptor.geometry.vertexIndices.count();
		const uint64 surfacePositionCount = descriptor.geometry.vertexComponents.count() / 3;
		const uint64 surfaceNormalCount = 1;
		const uint64 surfaceTextureCoordinateCount = descriptor.geometry.vertexTextureCoordinateComponents.count() / 2;

		IntermediateMaterial& material = getMaterial(descriptor.material);
		IntermediateMaterialGeometry& materialGeometry = material.geometries[0];
		IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

		if (geometry.sharedInstanceCount == 0) {
			const BuildTransform3D instance = {
				.translation = {0.0f, 0.0f, 0.0f},
				.rotation = {0.0f, 0.0f, 0.0f},
				.scale = 1.0f,
			};

			materialGeometry.instances.emplace_back(instance);

			geometry.sharedInstanceCount += 1;
			staticInstanceCount += 1;
		}

		const uint64 vertexOffset = geometry.vertices.count();
		const uint32 positionOffset = static_cast<uint32>(geometry.positions.count());
		const uint32 normalOffset = static_cast<uint32>(geometry.normals.count());
		const uint32 textureCoordinateOffset = static_cast<uint32>(geometry.textureCoordinates.count());

		resizeBuffer(geometry.vertices, surfaceVertexCount);
		resizeBuffer(geometry.positions, surfacePositionCount);
		resizeBuffer(geometry.normals, surfaceNormalCount);
		resizeBuffer(geometry.textureCoordinates, surfaceTextureCoordinateCount);
		resizeBuffer(geometry.transformedTextureCoordinates, surfaceTextureCoordinateCount);

		const quat<float32> orientation = quat<float32>::axisAngle(transform.axis, transform.angle);
		const quat<float32> inverseOrientation = inverse(orientation);
		const mat4x4<float32> worldTransform = (
			mat4x4<float32>(orientation) *
			mat4x4<float32>::translation(transform.translation)
		);
		const mat3x3<float32> textureTransform = (
			mat3x3<float32>::scaling(descriptor.geometry.scale / descriptor.textureFiltering.scale) *
			mat3x3<float32>::rotation(descriptor.textureFiltering.rotation * pi<float32>()) *
			mat3x3<float32>::translation(descriptor.textureFiltering.translation)
		);

		svec2<float32> maxCoordinate = {min<float32>(), min<float32>()};
		svec2<float32> minCoordinate = {max<float32>(), max<float32>()};

		uint32 surfaceVertexIndex = 0;

		while (surfaceVertexIndex < surfaceVertexCount) {
			geometry.vertices[vertexOffset + surfaceVertexIndex] = {
				.positionIndex = positionOffset + descriptor.geometry.vertexIndices[surfaceVertexIndex],
				.normalIndex = normalOffset,
				.textureCoordinateIndex = textureCoordinateOffset + descriptor.geometry.vertexTextureCoordinateIndices[surfaceVertexIndex],
			};

			surfaceVertexIndex += 1;
		}

		uint32 surfacePositionIndex = 0;

		while (surfacePositionIndex < surfacePositionCount) {
			// TODO(lyah/scalar)
			geometry.positions[positionOffset + surfacePositionIndex] = svec3<float32>(multiplyTransform({
				descriptor.geometry.vertexComponents[surfacePositionIndex * 3 + 0],
				descriptor.geometry.vertexComponents[surfacePositionIndex * 3 + 1],
				descriptor.geometry.vertexComponents[surfacePositionIndex * 3 + 2],
			}, worldTransform));

			surfacePositionIndex += 1;
		}

		// TODO(lyah/scalar)
		geometry.normals[normalOffset] = svec3<float32>(normalized(descriptor.geometry.normal * inverseOrientation));

		uint32 surfaceTextureCoordinateIndex = 0;

		while (surfaceTextureCoordinateIndex < surfaceTextureCoordinateCount) {
			const svec2<float32> textureCoordinate = {
				descriptor.geometry.vertexTextureCoordinateComponents[surfaceTextureCoordinateIndex * 2 + 0],
				descriptor.geometry.vertexTextureCoordinateComponents[surfaceTextureCoordinateIndex * 2 + 1],
			};

			// TODO(lyah/scalar)
			geometry.textureCoordinates[textureCoordinateOffset + surfaceTextureCoordinateIndex] = textureCoordinate;
			geometry.transformedTextureCoordinates[textureCoordinateOffset + surfaceTextureCoordinateIndex] = svec2<float32>(multiplyTransform(vec2<float32>(textureCoordinate), textureTransform));

			maxCoordinate.x = max(maxCoordinate.x, textureCoordinate.x);
			maxCoordinate.y = max(maxCoordinate.y, textureCoordinate.y);
			minCoordinate.x = min(minCoordinate.x, textureCoordinate.x);
			minCoordinate.y = min(minCoordinate.y, textureCoordinate.y);

			surfaceTextureCoordinateIndex += 1;
		}

		Lightmap lightmap{};
		lightmap.geometryIndex = materialGeometry.geometryIndex;
		lightmap.vertexRange = {vertexOffset, surfaceVertexCount};
		lightmap.scale = (maxCoordinate - minCoordinate) * descriptor.geometry.scale;
		lightmap.minCoordinate = minCoordinate;
		lightmap.luxelScale = descriptor.lightmap.luxelScale;
		lightmap.tintColor = descriptor.lightmap.tintColor;
		lightmap.emissiveColor = descriptor.lightmap.emissiveColor;
		lightmap.emissiveIntensity = descriptor.lightmap.emissiveIntensity;

		lightmaps.emplace_back(lightmap);

		bakedVertexCount += surfaceVertexCount;
		bakedPositionCount += surfacePositionCount;
		bakedNormalCount += surfaceNormalCount;
		bakedTextureCoordinateCount += surfaceTextureCoordinateCount;

		staticVertexCount += surfaceVertexCount;
		staticPositionCount += surfacePositionCount;
		staticNormalCount += surfaceNormalCount;
		staticTextureCoordinateCount += surfaceTextureCoordinateCount;

		// TODO(asset-builder/scene/lightmap): Create less coordinates (1 per texture coordinate per instance).
		lightmapCoordinateCount += surfaceVertexCount;
	}
}