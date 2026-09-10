// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"

#include "Engine/Base/Type/float16.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	inline uint32 getOffsetIndex(const IntermediateGeometry& geometry) {
		if (geometry.assetIdentifier == max<uint32>()) {
			return 0;
		}

		return 1 + !geometry.staticUsage;
	}

	inline void countGeometries(view<IntermediateGeometry> geometries, uint32 geometryCounts[2], uint32 vertexCounts[3], uint32 indexCounts[3]) {
		for (const IntermediateGeometry& geometry : geometries) {
			if (geometry.sharedInstanceCount == 0) {
				continue;
			}

			geometryCounts[!geometry.staticUsage] += 1;

			const uint32 offsetIndex = getOffsetIndex(geometry);

			vertexCounts[offsetIndex] += static_cast<uint32>(geometry.vertices.count());
			indexCounts[offsetIndex] += static_cast<uint32>(geometry.indices.count());
		}
	}

	inline void countMaterialRelatedObjects(view<IntermediateGeometry> geometries, view<IntermediateMaterialShader> materialShaders, uint32 materialShaderCounts[2], uint32& materialCount, uint32& materialGeometryCount, uint32& staticInstanceCount) {
		for (const IntermediateMaterialShader& materialShader : materialShaders) {
			const uint32 previousMaterialCount = materialCount;

			for (const IntermediateMaterial& material : materialShader.materials) {
				const uint32 previousMaterialGeometryCount = materialGeometryCount;

				for (const IntermediateMaterialGeometry& materialGeometry : material.geometries) {
					if (materialGeometry.instances.size() == 0) {
						continue;
					}

					if (materialShader.asset.geometryType == GeometryType::STATIC) {
						const IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

						staticInstanceCount += static_cast<uint32>(materialGeometry.instances.size());
					}

					materialGeometryCount += 1;
				}

				if (materialGeometryCount == previousMaterialGeometryCount) {
					continue;
				}

				materialCount += 1;
			}

			if (materialCount == previousMaterialCount) {
				continue;
			}

			materialShaderCounts[static_cast<uint32>(materialShader.asset.geometryType)] += 1;
		}
	}

	BuildScene3D SceneBuilder::build() {
		const view<IntermediateGeometry> preFilteredGeometries(geometries.data(), geometries.size());
		const view<IntermediateMaterialShader> preFilteredMaterialShaders(materialShaders.data(), materialShaders.size());

		uint32 geometryCounts[static_cast<uint32>(GeometryType::count)] = {0, 0};

		// Static (baked), static (external), dynamic.
		uint32 vertexCounts[] = {0, 0, 0};
		uint32 indexCounts[] = {0, 0, 0};

		countGeometries(preFilteredGeometries, geometryCounts, vertexCounts, indexCounts);

		uint32 materialShaderCounts[static_cast<uint32>(GeometryType::count)] = {0, 0};
		uint32 materialCount = 0;
		uint32 materialGeometryCount = 0;
		uint32 staticInstanceCount = 0;

		countMaterialRelatedObjects(preFilteredGeometries, preFilteredMaterialShaders, materialShaderCounts, materialCount, materialGeometryCount, staticInstanceCount);

		BuildScene3D scene{};
		scene.skyColor = descriptor.skyColor;
		scene.environmentMapImageAsset = descriptor.skyboxImageAsset;
		scene.sunDirection = descriptor.sunDirection;
		scene.sunColor = descriptor.sunColor * descriptor.sunIntensity;
		scene.vertices = sequence<BuildVertex3D>(vertexCounts[0] + vertexCounts[1] + vertexCounts[2]);
		scene.indices = sequence<uint32>(indexCounts[0] + indexCounts[1] + indexCounts[2]);
		scene.staticGeometry.geometryCount = geometryCounts[0];
		scene.staticGeometry.bakedVertexCount = vertexCounts[0];
		scene.staticGeometry.externalVertexCount = vertexCounts[1];
		scene.staticGeometry.bakedIndexCount = indexCounts[0];
		scene.staticGeometry.externalIndexCount = indexCounts[1];
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor = {
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {LIGHTMAP_ATLAS_EXTENT.width, LIGHTMAP_ATLAS_EXTENT.height, 1},
			.mipLevelCount = 1,
			.layerCount = 1,
			.data = sequence<uint8>(LIGHTMAP_ATLAS_EXTENT.width * LIGHTMAP_ATLAS_EXTENT.height * getByteSize(ImageFormat::R8G8B8A8_UNORM)),
		};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
		scene.dynamicGeometry.entityCount = entityCount;
		scene.spawn = descriptor.spawn;
		scene.geometries = sequence<BuildGeometry>(geometryCounts[0] + geometryCounts[1]);

		uint32 intermediateGeometryIndex = 0;
		uint32 buildGeometryOffsets[] = {0, geometryCounts[0]};
		uint32 vertexOffsets[] = {0, vertexCounts[0], vertexCounts[0] + vertexCounts[1]};
		uint32 indexOffsets[] = {0, indexCounts[0], indexCounts[0] + indexCounts[1]};

		while (intermediateGeometryIndex < geometries.size()) {
			IntermediateGeometry& intermediateGeometry = geometries[intermediateGeometryIndex];

			if (intermediateGeometry.sharedInstanceCount == 0) {
				intermediateGeometryIndex += 1;

				continue;
			}

			uint32& buildGeometryOffset = buildGeometryOffsets[!intermediateGeometry.staticUsage];

			const uint32 offsetIndex = getOffsetIndex(intermediateGeometry);

			uint32& vertexOffset = vertexOffsets[offsetIndex];
			uint32& indexOffset = indexOffsets[offsetIndex];

			BuildGeometry& geometry = scene.geometries[buildGeometryOffset];
			geometry.vertexRange.offset = vertexOffset;
			geometry.vertexRange.count = static_cast<uint32>(intermediateGeometry.vertices.count());
			geometry.indexRange.offset = indexOffset;
			geometry.indexRange.count = static_cast<uint32>(intermediateGeometry.indices.count());
			geometry.assetIdentifier = intermediateGeometry.assetIdentifier;

			intermediateGeometry.filteredIndex = buildGeometryOffset;

			std::copy(intermediateGeometry.vertices.begin(), intermediateGeometry.vertices.end(), &scene.vertices[geometry.vertexRange.offset]);

			uint32 indexIndex = 0;

			while (indexIndex < geometry.indexRange.count) {
				scene.indices[geometry.indexRange.offset + indexIndex] = geometry.vertexRange.offset + intermediateGeometry.indices[indexIndex];

				indexIndex += 1;
			}

			vertexOffset += geometry.vertexRange.count;
			indexOffset += geometry.indexRange.count;
			intermediateGeometryIndex += 1;
			buildGeometryOffset += 1;
		}

		scene.materialShaders = sequence<BuildMaterialShader>(materialShaderCounts[0] + materialShaderCounts[1]);
		scene.materials = sequence<BuildMaterial>(materialCount);
		scene.materialGeometries = sequence<MaterialGeometry>(materialGeometryCount);
		scene.staticGeometry.instances = sequence<BuildTransform3D>(staticInstanceCount);

		uint32 intermediateMaterialShaderIndex = 0;
		uint32 buildMaterialShaderOffsets[static_cast<uint32>(GeometryType::count)] = {0, materialShaderCounts[0]};
		uint32 buildMaterialOffset = 0;
		uint32 buildMaterialGeometryOffset = 0;
		uint32 buildInstanceOffset = 0;

		while (intermediateMaterialShaderIndex < materialShaders.size()) {
			const IntermediateMaterialShader& intermediateMaterialShader = materialShaders[intermediateMaterialShaderIndex];
			const uint32 currentMaterialOffset = buildMaterialOffset;

			uint32 intermediateMaterialIndex = 0;

			while (intermediateMaterialIndex < intermediateMaterialShader.materials.size()) {
				const IntermediateMaterial& intermediateMaterial = intermediateMaterialShader.materials[intermediateMaterialIndex];
				const uint32 currentMaterialGeometryOffset = buildMaterialGeometryOffset;

				uint32 intermediateMaterialGeometryIndex = 0;

				while (intermediateMaterialGeometryIndex < intermediateMaterial.geometries.size()) {
					const IntermediateMaterialGeometry& intermediateMaterialGeometry = intermediateMaterial.geometries[intermediateMaterialGeometryIndex];

					if (intermediateMaterialGeometry.instances.size() == 0) {
						intermediateMaterialGeometryIndex += 1;

						continue;
					}

					MaterialGeometry& materialGeometry = scene.materialGeometries[buildMaterialGeometryOffset];
					materialGeometry.instanceRange.offset = max<uint32>();
					materialGeometry.instanceRange.count = static_cast<uint32>(intermediateMaterialGeometry.instances.size());
					materialGeometry.geometryIndex = geometries[intermediateMaterialGeometry.geometryIndex].filteredIndex;

					if (intermediateMaterialShader.asset.geometryType == GeometryType::STATIC) {
						materialGeometry.instanceRange.offset = buildInstanceOffset;

						std::copy(intermediateMaterialGeometry.instances.begin(), intermediateMaterialGeometry.instances.end(), &scene.staticGeometry.instances[buildInstanceOffset]);

						buildInstanceOffset += materialGeometry.instanceRange.count;
					}

					buildMaterialGeometryOffset += 1;
					intermediateMaterialGeometryIndex += 1;
				}

				if (buildMaterialGeometryOffset - currentMaterialGeometryOffset == 0) {
					intermediateMaterialIndex += 1;

					continue;
				}

				BuildMaterial& material = scene.materials[buildMaterialOffset];
				material.assetIdentifier = intermediateMaterial.assetIdentifier;
				material.materialGeometryRange.offset = currentMaterialGeometryOffset;
				material.materialGeometryRange.count = buildMaterialGeometryOffset - currentMaterialGeometryOffset;

				intermediateMaterialIndex += 1;
				buildMaterialOffset += 1;
			}

			if (buildMaterialOffset - currentMaterialOffset == 0) {
				intermediateMaterialShaderIndex += 1;

				continue;
			}

			uint32& buildMaterialShaderOffset = buildMaterialShaderOffsets[static_cast<uint32>(intermediateMaterialShader.asset.geometryType)];

			BuildMaterialShader& materialShader = scene.materialShaders[buildMaterialShaderOffset];
			materialShader.asset = intermediateMaterialShader.asset;
			materialShader.materialRange.offset = currentMaterialOffset;
			materialShader.materialRange.count = buildMaterialOffset - currentMaterialOffset;

			intermediateMaterialShaderIndex += 1;
			buildMaterialShaderOffset += 1;
		}

		// TODO(asset-builder/scene/build): Refactor.
		for (MaterialGeometry& materialGeometry : scene.materialGeometries) {
			if (materialGeometry.instanceRange.offset == max<uint32>()) {
				materialGeometry.instanceRange.offset = buildInstanceOffset;

				buildInstanceOffset += materialGeometry.instanceRange.count;
			}
		}

		scene.dynamicGeometry.materialShaderOffset = materialShaderCounts[0];

		buildLights(scene.dynamicGeometry);
		buildEntityComponentMappings(scene.dynamicGeometry);
		buildLightmaps(scene);

		return scene;
	}

	void SceneBuilder::buildLightmaps(BuildScene3D& scene) {
		scene.staticGeometry.lightmapCoordinates = sequence<svec2<float32>>(lightmapVertexCount);

		lightmapLookups.reserve(lightmaps.size());

		if (lightmapVertexCount > 0) {
			createLightmaps(scene.staticGeometry);
			updateLightmapCoordinates(scene.staticGeometry);

			lightmapper.generateLightmapAtlas({
				.scene = &scene,
				.emissiveTriangleIndices = createEmissiveTriangleIndexBuffer(scene),
				.triangles = createTriangleBuffer(scene),
				.materials = view<LightmapMaterial>(lightmapMaterials.data(), lightmapMaterials.size()),
			});
		}
	}

	void SceneBuilder::buildLights(BuildDynamicSceneGeometry3D& dynamicGeometry) {
		dynamicGeometry.pointLights = sequence<PointLight>(pointLights.size());

		std::copy(pointLights.begin(), pointLights.end(), &dynamicGeometry.pointLights[0]);
	}

	void SceneBuilder::buildEntityComponentMappings(BuildDynamicSceneGeometry3D& dynamicGeometry) {
		dynamicGeometry.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappings.size());
		dynamicGeometry.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(rigidBodyMappings.size());

		std::copy(geometryMappings.begin(), geometryMappings.end(), &dynamicGeometry.geometryMappings[0]);
		std::copy(rigidBodyMappings.begin(), rigidBodyMappings.end(), &dynamicGeometry.rigidBodyMappings[0]);

		for (EntityComponentMapping<GeometryComponent>& geometryMapping : dynamicGeometry.geometryMappings) {
			geometryMapping.component.geometryIndex = geometries[geometryMapping.component.geometryIndex].filteredIndex;
		}
	}
}