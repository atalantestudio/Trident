// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	Face& SceneBuilder::createFace(const FaceDescriptor& descriptor) {
		ENGINE_ASSERT(descriptor.vertices.count() >= 3, "Not enough vertices.");

		const uint32 materialShaderIndex = descriptor.material >> 16;
		const uint32 materialIndex = descriptor.material & 0xff;

		IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

		ENGINE_ASSERT(materialShader.asset.geometryType == GeometryType::STATIC, "Cannot bind a static geometry face to a dynamic material shader.");

		IntermediateMaterial& material = materialShader.materials[materialIndex];

		if (material.bakedGeometryIndex >= material.geometries.size()) {
			const uint32 geometryIndex = static_cast<uint32>(geometries.size());

			IntermediateGeometry geometry{};
			geometry.assetIdentifier = max<uint32>();
			geometry.sharedInstanceCount = 1;
			geometry.staticUsage = true;

			geometries.emplace_back(geometry);

			BuildTransform3D instance{};
			instance.translation = {0.0f, 0.0f, 0.0f};
			instance.rotation = {0.0f, 0.0f, 0.0f};
			instance.scale = 1.0f;

			IntermediateMaterialGeometry materialGeometry{};
			materialGeometry.geometryIndex = geometryIndex;
			materialGeometry.instances.emplace_back(instance);

			material.bakedGeometryIndex = static_cast<uint32>(material.geometries.size());
			material.geometries.emplace_back(materialGeometry);

			materialGeometryCount += 1;
			staticInstanceCount += 1;
		}

		const IntermediateMaterialGeometry& materialGeometry = material.geometries[material.bakedGeometryIndex];

		IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

		Face face{};
		face.vertexCount = static_cast<uint32>(descriptor.vertices.count());
		face.indexRange.offset = staticGeometryFaceVertexCount;
		face.triangleOffset = staticGeometryFaceTriangleCount;
		face.normal = descriptor.normal;
		face.geometryIndex = materialGeometry.geometryIndex;
		face.material = descriptor.material;
		face.textureFiltering = descriptor.textureFiltering;
		face.boundingBox.min = {max<float32>(), max<float32>()};
		face.boundingBox.max = {min<float32>(), min<float32>()};
		face.lightmap = descriptor.lightmap;

		uint32 vertexIndex = 0;

		while (vertexIndex < face.vertexCount) {
			const svec3<float32> vertex = descriptor.vertices[vertexIndex];

			facePositions[facePositionCount + vertexIndex] = vertex;
			indices[staticGeometryFaceVertexCount + vertexIndex] = facePositionCount + vertexIndex;

			vertexIndex += 1;
		}

		faces[staticGeometryFaceCount] = face;

		geometry.faceIndices.emplace_back(staticGeometryFaceCount);

		staticGeometryFaceVertexCount += face.vertexCount;
		staticGeometryFaceTriangleCount += face.vertexCount - 2;
		staticGeometryFaceCount += 1;
		facePositionCount += face.vertexCount;
		lightmapCoordinateCount += face.vertexCount;
		lightmapCount += 1;

		return faces[staticGeometryFaceCount - 1];
	}

	void SceneBuilder::placeSurface(const SurfaceDescriptor& descriptor, const TransformAngleAxis3D& transform) {
		// TODO(asset-builder/map/surface): Deprecate surfaces in favor of static geometry faces.
		ENGINE_ERROR_UNSUPPORTED();

		const uint32 materialShaderIndex = descriptor.material >> 16;
		const uint32 materialIndex = descriptor.material & 0xff;

		IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

		ENGINE_ASSERT(materialShader.asset.geometryType == GeometryType::STATIC, "Cannot place a static surface with a dynamic material.");

		IntermediateMaterial& material = materialShader.materials[materialIndex];

		if (material.bakedGeometryIndex >= material.geometries.size()) {
			const uint32 geometryIndex = static_cast<uint32>(geometries.size());

			IntermediateGeometry geometry{};
			geometry.assetIdentifier = max<uint32>();
			geometry.sharedInstanceCount = 0;
			geometry.staticUsage = true;

			geometries.emplace_back(geometry);

			IntermediateMaterialGeometry materialGeometry{};
			materialGeometry.geometryIndex = geometryIndex;

			material.bakedGeometryIndex = static_cast<uint32>(material.geometries.size());
			material.geometries.emplace_back(materialGeometry);

			materialGeometryCount += 1;
		}

		IntermediateMaterialGeometry& materialGeometry = material.geometries[material.bakedGeometryIndex];
		IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

		if (geometry.sharedInstanceCount == 0) {
			const BuildInstance instance = {
				.transform = {
					.translation = {0.0f, 0.0f, 0.0f},
					.rotation = {0.0f, 0.0f, 0.0f},
					.scale = 1.0f,
				},
				.lightmap = descriptor.lightmap,
			};

			materialGeometry.instances.emplace_back(instance);

			geometry.sharedInstanceCount += 1;
			staticInstanceCount += 1;
		}

		const uint64 surfaceVertexCount = descriptor.geometry.vertices.count();
		const uint64 surfaceIndexCount = descriptor.geometry.indices.count();
		const uint32 vertexOffset = static_cast<uint32>(geometry.vertices.count());
		const uint64 indexOffset = geometry.indices.count();

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
			const svec3<float32> tangent = svec3<float32>(normalized(vec3<float32>(surfaceVertex.tangent[0], surfaceVertex.tangent[1], surfaceVertex.tangent[2]) * inverseOrientation));
			const uint32 initialTextureCoordinate = pack<VertexInputAttributeFormat::R16G16_FLOAT, uint32>(surfaceVertex.textureCoordinate);

			// TODO(lyah/scalar)
			BuildVertex3D& vertex = geometry.vertices[vertexOffset + vertexIndex];
			vertex.position = svec3<float32>(multiplyTransform(vec3<float32>(surfaceVertex.position), worldTransform));
			vertex.normal = svec3<float32>(normalized(vec3<float32>(surfaceVertex.normal) * inverseOrientation));
			vertex.tangent = {tangent.x, tangent.y, tangent.z, *pointer_cast<const float32*>(&initialTextureCoordinate)};
			vertex.textureCoordinate = svec2<float32>(multiplyTransform(vec2<float32>(surfaceVertex.textureCoordinate), textureTransform));

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

		bakedVertexCount += surfaceVertexCount;
		bakedIndexCount += surfaceIndexCount;
	}
}