// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	Face& MapBuilder::createFace(const FaceDescriptor& descriptor) {
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

			BuildInstance instance{};
			instance.transform.translation = {0.0f, 0.0f, 0.0f};
			instance.transform.rotation = {0.0f, 0.0f, 0.0f};
			instance.transform.scale = 1.0f;
			instance.lightmap.luxelScale = {1.0f, 1.0f};
			instance.lightmap.emissiveColor = {0.0f, 0.0f, 0.0f};
			instance.lightmap.emissiveIntensity = 0.0f;
			instance.lightmap.albedo = {1.0f, 1.0f, 1.0f};
			instance.lightmapScale = {1.0f, 1.0f};

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
}