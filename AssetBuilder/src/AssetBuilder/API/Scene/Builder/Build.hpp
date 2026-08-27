// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"

#include "Engine/Base/Type/float16.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	BuildScene3D SceneBuilder::build() {
		for (IntermediateMaterialShader& materialShader : staticMaterialShaders) {
			for (IntermediateMaterial& material : materialShader.materials) {
				const IntermediateMaterialGeometry& materialGeometry = material.geometries[0];

				ENGINE_ASSERT(materialGeometry.instances.size() > 0, "Baked geometry is unused.");
			}
		}

		BuildScene3D scene{};

		if (descriptor.skyboxImageAsset != nullptr) {
			scene.skyboxAssetIdentifier = descriptor.skyboxImageAsset->identifier;
		}
		else {
			scene.skyboxAssetIdentifier = max<uint32>();
		}

		scene.skyColor = descriptor.skyColor;
		scene.sunDirection = descriptor.sunDirection;
		scene.sunColor = descriptor.sunColor * descriptor.sunIntensity;
		scene.vertexCount = static_cast<uint32>(staticVertexCount + dynamicVertexCount);
		scene.staticGeometry.vertexCount = static_cast<uint32>(staticVertexCount);
		scene.triangleCount = static_cast<uint32>((staticIndexCount + dynamicIndexCount) / 3);
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = max<uint32>();
		scene.dynamicGeometry.entityCount = entityCount;

		buildGeometries(scene);
		buildMaterialGeometries(scene);
		buildMaterials(scene);
		buildMaterialShaders(scene);
		buildBakedGeometry(scene.staticGeometry);
		buildLights(scene.dynamicGeometry);
		buildEntityComponentMappings(scene.dynamicGeometry);
		buildLightmaps(scene);

		return scene;
	}

	void SceneBuilder::buildGeometries(BuildScene3D& scene) {
		scene.geometries = sequence<BuildGeometry>(geometries.size());

		uint32 indexOffset = 0;
		uint32 geometryIndex = 0;

		while (geometryIndex < scene.geometries.count()) {
			const IntermediateGeometry& intermediateGeometry = geometries[geometryIndex];

			BuildGeometry& geometry = scene.geometries[geometryIndex];
			geometry.assetIdentifier = intermediateGeometry.assetIdentifier;
			geometry.indexRange.count = static_cast<uint32>(intermediateGeometry.indices.count());

			if (geometry.assetIdentifier == 0) {
				geometry.indexRange.offset = indexOffset;

				indexOffset += geometry.indexRange.count;
			}

			geometryIndex += 1;
		}

		for (BuildGeometry& geometry : scene.geometries) {
			if (geometry.assetIdentifier > 0) {
				geometry.indexRange.offset = indexOffset;

				indexOffset += geometry.indexRange.count;
			}
		}
	}

	void SceneBuilder::buildMaterialGeometries(BuildScene3D& scene) {
		scene.materialGeometries = sequence<BuildMaterialGeometry>(materialGeometryCount);
		scene.staticGeometry.instances = sequence<BuildTransform3D>(staticInstanceCount);

		uint32 instanceOffset = 0;
		uint32 materialGeometryOffset = 0;

		for (const IntermediateMaterialShader& materialShader : staticMaterialShaders) {
			for (const IntermediateMaterial& material : materialShader.materials) {
				uint32 materialGeometryIndex = 0;

				while (materialGeometryIndex < material.geometries.size()) {
					const IntermediateMaterialGeometry& intermediateMaterialGeometry = material.geometries[materialGeometryIndex];

					BuildMaterialGeometry& materialGeometry = scene.materialGeometries[materialGeometryOffset + materialGeometryIndex];
					materialGeometry.instanceRange.offset = instanceOffset;
					materialGeometry.instanceRange.count = static_cast<uint32>(intermediateMaterialGeometry.instances.size());
					materialGeometry.geometryIndex = intermediateMaterialGeometry.geometryIndex;

					std::copy(intermediateMaterialGeometry.instances.begin(), intermediateMaterialGeometry.instances.end(), &scene.staticGeometry.instances[instanceOffset]);

					instanceOffset += materialGeometry.instanceRange.count;
					materialGeometryIndex += 1;
				}

				materialGeometryOffset += materialGeometryIndex;
			}
		}

		for (const IntermediateMaterialShader& materialShader : dynamicMaterialShaders) {
			for (const IntermediateMaterial& material : materialShader.materials) {
				uint32 materialGeometryIndex = 0;

				while (materialGeometryIndex < material.geometries.size()) {
					const IntermediateMaterialGeometry& intermediateMaterialGeometry = material.geometries[materialGeometryIndex];

					BuildMaterialGeometry& materialGeometry = scene.materialGeometries[materialGeometryOffset + materialGeometryIndex];
					materialGeometry.instanceRange.offset = instanceOffset;
					materialGeometry.instanceRange.count = static_cast<uint32>(intermediateMaterialGeometry.instances.size());
					materialGeometry.geometryIndex = intermediateMaterialGeometry.geometryIndex;

					instanceOffset += materialGeometry.instanceRange.count;
					materialGeometryIndex += 1;
				}

				materialGeometryOffset += materialGeometryIndex;
			}
		}
	}

	void SceneBuilder::buildMaterials(BuildScene3D& scene) {
		scene.materials = sequence<BuildMaterial>(materialCount);

		uint64 materialOffset = 0;

		for (const IntermediateMaterialShader& materialShader : staticMaterialShaders) {
			uint32 materialIndex = 0;

			while (materialIndex < materialShader.materials.size()) {
				const IntermediateMaterial& intermediateMaterial = materialShader.materials[materialIndex];

				BuildMaterial& material = scene.materials[materialOffset + materialIndex];
				material.assetIdentifier = intermediateMaterial.assetIdentifier;
				material.geometryCount = static_cast<uint32>(intermediateMaterial.geometries.size());

				materialIndex += 1;
			}

			materialOffset += materialShader.materials.size();
		}

		for (const IntermediateMaterialShader& materialShader : dynamicMaterialShaders) {
			uint32 materialIndex = 0;

			while (materialIndex < materialShader.materials.size()) {
				const IntermediateMaterial& intermediateMaterial = materialShader.materials[materialIndex];

				BuildMaterial& material = scene.materials[materialOffset + materialIndex];
				material.assetIdentifier = intermediateMaterial.assetIdentifier;
				material.geometryCount = static_cast<uint32>(intermediateMaterial.geometries.size());

				materialIndex += 1;
			}

			materialOffset += materialShader.materials.size();
		}
	}

	void SceneBuilder::buildMaterialShaders(BuildScene3D& scene) {
		scene.staticGeometry.materialShaders = sequence<BuildMaterialShader>(staticMaterialShaders.size());
		scene.dynamicGeometry.materialShaders = sequence<BuildMaterialShader>(dynamicMaterialShaders.size());

		uint32 materialOffset = 0;
		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < scene.staticGeometry.materialShaders.count()) {
			const IntermediateMaterialShader& intermediateMaterialShader = staticMaterialShaders[materialShaderIndex];

			BuildMaterialShader& materialShader = scene.staticGeometry.materialShaders[materialShaderIndex];
			materialShader.asset = getMaterialShaderModuleAsset(intermediateMaterialShader.assetIdentifier);
			materialShader.materialRange.offset = materialOffset;
			materialShader.materialRange.count = static_cast<uint32>(intermediateMaterialShader.materials.size());

			materialOffset += materialShader.materialRange.count;
			materialShaderIndex += 1;
		}

		materialShaderIndex = 0;

		while (materialShaderIndex < scene.dynamicGeometry.materialShaders.count()) {
			const IntermediateMaterialShader& intermediateMaterialShader = dynamicMaterialShaders[materialShaderIndex];

			BuildMaterialShader& materialShader = scene.dynamicGeometry.materialShaders[materialShaderIndex];
			materialShader.asset = getMaterialShaderModuleAsset(intermediateMaterialShader.assetIdentifier);
			materialShader.materialRange.offset = materialOffset;
			materialShader.materialRange.count = static_cast<uint32>(intermediateMaterialShader.materials.size());

			materialOffset += materialShader.materialRange.count;
			materialShaderIndex += 1;
		}
	}

	void SceneBuilder::buildBakedGeometry(BuildStaticSceneGeometry3D& staticGeometry) {
		staticGeometry.bakedVertices = sequence<BuildVertex3D>(bakedVertexCount);
		staticGeometry.bakedIndices = sequence<uint32>(bakedIndexCount);

		uint32 bakedVertexOffset = 0;
		uint64 bakedIndexOffset = 0;

		while (bakedVertexOffset < bakedVertexCount) {
			uint32 geometryIndex = 0;

			while (geometryIndex < geometries.size()) {
				const IntermediateGeometry& geometry = geometries[geometryIndex];

				if (geometry.assetIdentifier > 0 || geometry.vertices.count() == 0) {
					geometryIndex += 1;

					continue;
				}

				uint32 vertexIndex = 0;

				while (vertexIndex < geometry.vertices.count()) {
					BuildVertex3D& vertex = staticGeometry.bakedVertices[bakedVertexOffset + vertexIndex];
					vertex = geometry.vertices[vertexIndex];

					const float32 w = vertex.tangent[3];

					vertex.textureCoordinate = unpack<VertexInputAttributeFormat::R16G16_FLOAT, svec2<float32>>(*pointer_cast<const uint32*>(&w));

					vertexIndex += 1;
				}

				uint32 indexIndex = 0;

				while (indexIndex < geometry.indices.count()) {
					staticGeometry.bakedIndices[bakedIndexOffset + indexIndex] = bakedVertexOffset + geometry.indices[indexIndex];

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
	}
}