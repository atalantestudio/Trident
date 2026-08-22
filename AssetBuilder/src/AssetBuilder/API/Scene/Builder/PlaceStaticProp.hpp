// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::placeStaticProp(const MeshDescriptor& descriptor) {
		IntermediateMaterial& material = getMaterial(descriptor.material);

		const uint32 materialGeometryIndex = findMaterialGeometryIndex(material, descriptor.geometryIndex);

		if (materialGeometryIndex >= material.geometries.size()) {
			const IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];

			IntermediateMaterialGeometry materialGeometry{};
			materialGeometry.geometryIndex = descriptor.geometryIndex;

			material.geometries.emplace_back(materialGeometry);

			staticVertexCount += geometry.vertices.count();
			staticIndexCount += geometry.indices.count();
			materialGeometryCount += 1;
		}

		IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];
		geometry.sharedInstanceCount += 1;

		IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];
		svec2<float32> maxCoordinate = {min<float32>(), min<float32>()};
		svec2<float32> minCoordinate = {max<float32>(), max<float32>()};
		uint32 vertexIndex = 0;

		while (vertexIndex < geometry.vertices.count()) {
			// TODO(lyah/scalar)
			const svec2<float32> textureCoordinate = geometry.vertices[vertexIndex].textureCoordinate;

			maxCoordinate.x = max(maxCoordinate.x, textureCoordinate.x);
			maxCoordinate.y = max(maxCoordinate.y, textureCoordinate.y);
			minCoordinate.x = min(minCoordinate.x, textureCoordinate.x);
			minCoordinate.y = min(minCoordinate.y, textureCoordinate.y);

			vertexIndex += 1;
		}

		const BuildTransform3D instance = {
			.translation = svec3<float32>(descriptor.transform.translation),
			.rotation = svec3<float32>(descriptor.transform.rotation),
			.scale = descriptor.transform.scale,
		};

		materialGeometry.instances.emplace_back(instance);

		Lightmap lightmap{};
		lightmap.geometryIndex = materialGeometry.geometryIndex;
		lightmap.vertexRange = {0, geometry.vertices.count()};
		lightmap.triangleOffset = 0;
		lightmap.scale = descriptor.lightmapScale;
		lightmap.maxCoordinate = maxCoordinate;
		lightmap.minCoordinate = minCoordinate;
		lightmap.luxelScale = descriptor.lightmap.luxelScale;

		LightmapMaterial lightmapMaterial{};
		lightmapMaterial.emissiveColor = descriptor.lightmap.emissiveColor;
		lightmapMaterial.emissiveIntensity = descriptor.lightmap.emissiveIntensity;
		lightmapMaterial.albedo = descriptor.lightmap.albedo;

		lightmaps.emplace_back(lightmap);
		lightmapMaterials.emplace_back(lightmapMaterial);

		lightmapVertexCount += geometry.vertices.count();
		lightmapIndexCount += geometry.indices.count();
		staticInstanceCount += 1;
	}
}