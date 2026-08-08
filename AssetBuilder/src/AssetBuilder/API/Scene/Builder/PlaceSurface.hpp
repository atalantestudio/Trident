// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::placeSurface(const SurfaceDescriptor& descriptor, const TransformAngleAxis3D& transform) {
		const uint64 surfaceVertexCount = descriptor.geometry.vertices.count();
		const uint64 surfaceIndexCount = descriptor.geometry.indices.count();

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

		const uint32 vertexOffset = static_cast<uint32>(geometry.vertices.count());
		const uint64 indexOffset = geometry.indices.count();

		resizeBuffer(geometry.vertices, surfaceVertexCount);
		resizeBuffer(geometry.indices, surfaceIndexCount);

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

		uint32 vertexIndex = 0;

		while (vertexIndex < surfaceVertexCount) {
			const BuildVertex3D& surfaceVertex = descriptor.geometry.vertices[vertexIndex];

			BuildVertex3D& vertex = geometry.vertices[vertexOffset + vertexIndex];

			// TODO(lyah/scalar)
			vertex.position = svec3<float32>(multiplyTransform(vec3<float32>(surfaceVertex.position), worldTransform));
			vertex.normal = svec3<float32>(normalized(vec3<float32>(surfaceVertex.normal) * inverseOrientation));
			vertex.tangent = svec3<float32>(normalized(vec3<float32>(surfaceVertex.tangent) * inverseOrientation));

			// TODO(asset-builder/lightmapper)
			// vertex.textureCoordinate = svec2<float32>(multiplyTransform(vec2<float32>(surfaceVertex.textureCoordinate), textureTransform));
			vertex.textureCoordinate = surfaceVertex.textureCoordinate;

			maxCoordinate.x = max(maxCoordinate.x, surfaceVertex.textureCoordinate.x);
			maxCoordinate.y = max(maxCoordinate.y, surfaceVertex.textureCoordinate.y);
			minCoordinate.x = min(minCoordinate.x, surfaceVertex.textureCoordinate.x);
			minCoordinate.y = min(minCoordinate.y, surfaceVertex.textureCoordinate.y);

			vertexIndex += 1;
		}

		uint32 indexIndex = 0;

		while (indexIndex < surfaceIndexCount) {
			geometry.indices[indexOffset + indexIndex] = vertexOffset + descriptor.geometry.indices[indexIndex];

			indexIndex += 1;
		}

		Lightmap lightmap{};
		lightmap.geometryIndex = materialGeometry.geometryIndex;
		lightmap.vertexRange = {vertexOffset, surfaceVertexCount};
		lightmap.scale = descriptor.geometry.scale;
		lightmap.maxCoordinate = maxCoordinate;
		lightmap.minCoordinate = minCoordinate;
		lightmap.luxelScale = descriptor.lightmap.luxelScale;
		lightmap.tintColor = descriptor.lightmap.tintColor;
		lightmap.emissiveColor = descriptor.lightmap.emissiveColor;
		lightmap.emissiveIntensity = descriptor.lightmap.emissiveIntensity;

		lightmaps.emplace_back(lightmap);

		bakedVertexCount += surfaceVertexCount;
		bakedIndexCount += surfaceIndexCount;
		staticVertexCount += surfaceVertexCount;
		staticIndexCount += surfaceIndexCount;
		lightmapVertexCount += surfaceVertexCount;
		lightmapIndexCount += surfaceIndexCount;
	}
}