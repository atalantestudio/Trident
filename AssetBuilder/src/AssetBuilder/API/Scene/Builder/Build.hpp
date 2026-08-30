// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"

#include "Engine/Base/Type/float16.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	inline void countGeometries(view<IntermediateGeometry> geometries, uint32& geometryCount) {
		for (const IntermediateGeometry& geometry : geometries) {
			if (geometry.sharedInstanceCount == 0) {
				continue;
			}

			geometryCount += 1;
		}
	}

	inline void countMaterialRelatedObjects(view<IntermediateMaterialShader> materialShaders, uint32 materialShaderCounts[2], uint32& materialCount, uint32& materialGeometryCount, uint32& staticInstanceCount) {
		for (const IntermediateMaterialShader& materialShader : materialShaders) {
			const uint32 previousMaterialCount = materialCount;

			for (const IntermediateMaterial& material : materialShader.materials) {
				const uint32 previousMaterialGeometryCount = materialGeometryCount;

				for (const IntermediateMaterialGeometry& materialGeometry : material.geometries) {
					if (materialGeometry.instances.size() == 0) {
						continue;
					}

					if (materialShader.asset.geometryType == GeometryType::STATIC) {
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

		uint32 geometryCount = 0;
		uint32 materialShaderCounts[static_cast<uint32>(GeometryType::count)] = {0, 0};
		uint32 materialCount = 0;
		uint32 materialGeometryCount = 0;
		uint32 staticInstanceCount = 0;

		countGeometries(preFilteredGeometries, geometryCount);
		countMaterialRelatedObjects(preFilteredMaterialShaders, materialShaderCounts, materialCount, materialGeometryCount, staticInstanceCount);

		BuildScene3D scene{};
		scene.skyColor = descriptor.skyColor;

		if (descriptor.skyboxImageAsset != nullptr) {
			scene.skyboxAssetIdentifier = descriptor.skyboxImageAsset->identifier;
		}
		else {
			scene.skyboxAssetIdentifier = max<uint32>();
		}

		scene.sunDirection = descriptor.sunDirection;
		scene.sunColor = descriptor.sunColor * descriptor.sunIntensity;
		scene.vertexCount = static_cast<uint32>(staticVertexCount + dynamicVertexCount);
		scene.staticGeometry.vertexCount = static_cast<uint32>(staticVertexCount);
		scene.triangleCount = static_cast<uint32>((staticIndexCount + dynamicIndexCount) / 3);
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = max<uint32>();
		scene.dynamicGeometry.entityCount = entityCount;
		scene.spawn = descriptor.spawn;
		scene.geometries = sequence<BuildGeometry>(geometryCount);

		uint32 indexOffset = 0;
		uint32 intermediateGeometryIndex = 0;
		uint32 buildGeometryOffset = 0;

		while (intermediateGeometryIndex < geometries.size()) {
			IntermediateGeometry& intermediateGeometry = geometries[intermediateGeometryIndex];

			if (intermediateGeometry.sharedInstanceCount == 0) {
				intermediateGeometryIndex += 1;

				continue;
			}

			BuildGeometry& geometry = scene.geometries[buildGeometryOffset];
			geometry.indexRange.offset = 0;
			geometry.indexRange.count = static_cast<uint32>(intermediateGeometry.indices.count());
			geometry.assetIdentifier = intermediateGeometry.assetIdentifier;

			intermediateGeometry.filteredIndex = buildGeometryOffset;

			// TODO(wip)
			if (geometry.assetIdentifier == max<uint32>()) {
				geometry.indexRange.offset = indexOffset;

				indexOffset += geometry.indexRange.count;
			}

			intermediateGeometryIndex += 1;
			buildGeometryOffset += 1;
		}

		// TODO(wip): Refactor.
		for (BuildGeometry& geometry : scene.geometries) {
			if (geometry.assetIdentifier != max<uint32>()) {
				geometry.indexRange.offset = indexOffset;

				indexOffset += geometry.indexRange.count;
			}
		}

		// TODO(wip): Merge with buildGeometries.
		buildBakedGeometry(scene);

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
				material.geometryCount = static_cast<uint32>(intermediateMaterial.geometries.size());

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

		// TODO(wip): Refactor.
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

	void SceneBuilder::buildBakedGeometry(BuildScene3D& scene) {
		scene.staticGeometry.bakedVertices = sequence<BuildVertex3D>(bakedVertexCount);
		scene.staticGeometry.bakedIndices = sequence<uint32>(bakedIndexCount);

		uint32 bakedVertexOffset = 0;
		uint64 bakedIndexOffset = 0;

		while (bakedVertexOffset < bakedVertexCount) {
			uint32 geometryIndex = 0;

			while (geometryIndex < geometries.size()) {
				const IntermediateGeometry& geometry = geometries[geometryIndex];

				if (geometry.assetIdentifier != max<uint32>() || geometry.vertices.count() == 0 || geometry.sharedInstanceCount == 0) {
					geometryIndex += 1;

					continue;
				}

				uint32 vertexIndex = 0;

				while (vertexIndex < geometry.vertices.count()) {
					BuildVertex3D& vertex = scene.staticGeometry.bakedVertices[bakedVertexOffset + vertexIndex];
					vertex = geometry.vertices[vertexIndex];

					const float32 w = vertex.tangent[3];

					vertex.textureCoordinate = unpack<VertexInputAttributeFormat::R16G16_FLOAT, svec2<float32>>(*pointer_cast<const uint32*>(&w));

					vertexIndex += 1;
				}

				uint32 indexIndex = 0;

				while (indexIndex < geometry.indices.count()) {
					scene.staticGeometry.bakedIndices[bakedIndexOffset + indexIndex] = bakedVertexOffset + geometry.indices[indexIndex];

					indexIndex += 1;
				}

				bakedVertexOffset += static_cast<uint32>(geometry.vertices.count());
				bakedIndexOffset += geometry.indices.count();
				geometryIndex += 1;
			}
		}
	}

	void SceneBuilder::buildLightmaps(BuildScene3D& scene) {
		scene.staticGeometry.lightmapCoordinates = sequence<svec2<float32>>(lightmapVertexCount);

		lightmapLookups.reserve(lightmaps.size());

		if (lightmapVertexCount > 0) {
			createLightmaps(scene.staticGeometry);
			updateLightmapCoordinates(scene.staticGeometry);

			sequence<BuildVertex3D> vertices;
			sequence<uint32> indices;
			sequence<LightmapMaterial> materials;
			sequence<uint32> emissiveTriangleIndices;

			createLightmapperGeometryBuffers(scene, vertices, indices, materials, emissiveTriangleIndices);

			scene.staticGeometry.lightmapAtlasImageAssetDescriptor = {
				.format = ImageFormat::R8G8B8A8_UNORM,
				.extent = {LIGHTMAP_ATLAS_EXTENT.width, LIGHTMAP_ATLAS_EXTENT.height, 1},
				.mipLevelCount = 1,
				.layerCount = 1,
				.data = sequence<uint8>(LIGHTMAP_ATLAS_EXTENT.width * LIGHTMAP_ATLAS_EXTENT.height * getByteSize(ImageFormat::R8G8B8A8_UNORM)),
			};
			scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();

			lightmapper.generateAtlas(scene.staticGeometry.lightmapAtlasImageAssetDescriptor, {
				.vertices = vertices,
				.lightmapCoordinates = scene.staticGeometry.lightmapCoordinates,
				.indices = indices,
				.materials = materials,
				.emissiveTriangleIndices = emissiveTriangleIndices,
				.sceneDescriptor = descriptor,
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