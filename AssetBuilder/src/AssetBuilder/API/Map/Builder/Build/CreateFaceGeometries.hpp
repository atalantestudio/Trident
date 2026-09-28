// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	void MapBuilder::createFaceGeometries() {
		const uint64 faceGeometryOffset = geometries.size();

		uint32 faceIndex = 0;

		while (faceIndex < staticGeometryFaceCount) {
			Face& face = faces[faceIndex];

			const uint32 materialShaderIndex = face.material >> 16;
			const uint32 materialIndex = face.material & 0xff;

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

			face.geometryIndex = materialGeometry.geometryIndex;

			calculateFaceBoundingBoxes(face);

			IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];
			geometry.vertices = sequence<BuildVertex3D>(geometry.vertices.count() + face.getVertexCount());
			geometry.indices = sequence<uint32>(geometry.indices.count() + face.getTriangleCount() * 3);
			geometry.faceIndices.emplace_back(faceIndex);

			staticGeometryFaceTriangleCount += face.getTriangleCount();
			staticGeometryFaceVertexCount += face.getVertexCount();

			lightmapCount += 1;
			lightmapCoordinateCount += face.getVertexCount();

			faceIndex += 1;
		}

		uint64 geometryIndex = faceGeometryOffset;

		while (geometryIndex < geometries.size()) {
			IntermediateGeometry& geometry = geometries[geometryIndex];

			DEBUG_ASSERT(geometry.assetIdentifier == max<uint32>() && geometry.staticUsage);

			triangulateFaces(geometry, 0);

			geometryIndex += 1;
		}
	}
}