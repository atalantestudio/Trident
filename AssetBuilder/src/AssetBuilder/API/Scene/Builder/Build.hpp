// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	BuildScene3D SceneBuilder::build() {
		for (IntermediateMaterialShader& materialShader : staticMaterialShaders) {
			for (IntermediateMaterial& material : materialShader.materials) {
				const IntermediateMaterialGeometry& materialGeometry = material.geometries[0];

				ENGINE_ASSERT(materialGeometry.instances.size() > 0, "Baked geometry is unused.");
			}
		}

		BuildScene3D scene{};
		scene.triangleCount = static_cast<uint32>((staticVertexCount + dynamicVertexCount) / 3);
		scene.positionCount = static_cast<uint32>(staticPositionCount + dynamicPositionCount);
		scene.normalCount = static_cast<uint32>(staticNormalCount + dynamicNormalCount);
		scene.textureCoordinateCount = static_cast<uint32>(staticTextureCoordinateCount + dynamicTextureCoordinateCount);
		scene.skyColor = lightmapperArguments.skyColor;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = max<uint32>();
		scene.dynamicGeometry.entityCount = entityCount;

		buildGeometries(scene);
		buildMaterialGeometries(scene);
		buildMaterials(scene);
		buildMaterialShaders(scene);
		buildBakedGeometry(scene.staticGeometry);
		buildEntityComponentMappings(scene.dynamicGeometry);
		buildLightmaps(scene);

		return scene;
	}

	void SceneBuilder::buildGeometries(BuildScene3D& scene) {
		scene.geometries = sequence<BuildGeometry>(geometries.size());

		uint32 vertexOffset = 0;
		uint32 geometryIndex = 0;

		while (geometryIndex < scene.geometries.count()) {
			const IntermediateGeometry& intermediateGeometry = geometries[geometryIndex];

			BuildGeometry& geometry = scene.geometries[geometryIndex];
			geometry.assetIdentifier = intermediateGeometry.assetIdentifier;
			geometry.vertexRange.count = static_cast<uint32>(intermediateGeometry.vertices.count());

			if (geometry.assetIdentifier == 0) {
				geometry.vertexRange.offset = vertexOffset;

				vertexOffset += geometry.vertexRange.count;
			}

			geometryIndex += 1;
		}

		for (BuildGeometry& geometry : scene.geometries) {
			if (geometry.assetIdentifier > 0) {
				geometry.vertexRange.offset = vertexOffset;

				vertexOffset += geometry.vertexRange.count;
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
		staticGeometry.bakedVertices = sequence<GeometryIndex3D>(bakedVertexCount);
		staticGeometry.bakedPositions = sequence<svec3<float32>>(bakedPositionCount);
		staticGeometry.bakedNormals = sequence<svec3<float32>>(bakedNormalCount);
		staticGeometry.bakedTextureCoordinates = sequence<svec2<float32>>(bakedTextureCoordinateCount);

		uint64 bakedVertexOffset = 0;
		uint64 bakedPositionOffset = 0;
		uint64 bakedNormalOffset = 0;
		uint64 bakedTextureCoordinateOffset = 0;

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
					const GeometryIndex3D intermediateVertex = geometry.vertices[vertexIndex];

					GeometryIndex3D& vertex = staticGeometry.bakedVertices[bakedVertexOffset + vertexIndex];
					vertex.positionIndex = static_cast<uint32>(bakedPositionOffset) + intermediateVertex.positionIndex;
					vertex.normalIndex = static_cast<uint32>(bakedNormalOffset) + intermediateVertex.normalIndex;
					vertex.textureCoordinateIndex = static_cast<uint32>(bakedTextureCoordinateOffset) + intermediateVertex.textureCoordinateIndex;

					vertexIndex += 1;
				}

				std::copy(geometry.positions.begin(), geometry.positions.end(), &staticGeometry.bakedPositions[bakedPositionOffset]);
				std::copy(geometry.normals.begin(), geometry.normals.end(), &staticGeometry.bakedNormals[bakedNormalOffset]);
				std::copy(geometry.transformedTextureCoordinates.begin(), geometry.transformedTextureCoordinates.end(), &staticGeometry.bakedTextureCoordinates[bakedTextureCoordinateOffset]);

				bakedVertexOffset += geometry.vertices.count();
				bakedPositionOffset += geometry.positions.count();
				bakedNormalOffset += geometry.normals.count();
				bakedTextureCoordinateOffset += geometry.textureCoordinates.count();

				geometryIndex += 1;
			}
		}
	}

	void SceneBuilder::buildLightmaps(BuildScene3D& scene) {
		scene.staticGeometry.lightmapCoordinates = sequence<svec2<float32>>(lightmapCoordinateCount);

		lightmapLookups.reserve(lightmaps.size());

		if (lightmapCoordinateCount > 0) {
			createLightmaps(scene);
			updateLightmapCoordinates(scene.staticGeometry);

			sequence<GeometryIndex3D> vertices;
			sequence<svec3<float32>> positions;
			sequence<svec3<float32>> normals;

			createLightmapperGeometryBuffers(scene, vertices, positions, normals);

			scene.staticGeometry.lightmapAtlasImageAssetDescriptor = {
				.format = ImageFormat::R8G8B8A8_UNORM,
				.extent = {LIGHTMAP_ATLAS_EXTENT.width, LIGHTMAP_ATLAS_EXTENT.height, 1},
				.mipLevelCount = 1,
				.layerCount = 1,
				.data = sequence<uint8>(LIGHTMAP_ATLAS_EXTENT.width * LIGHTMAP_ATLAS_EXTENT.height * getByteSize(ImageFormat::R8G8B8A8_UNORM)),
			};
			scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();

			lightmapper.buildLightmapAtlas(
				vertices,
				positions,
				normals,
				scene.staticGeometry.lightmapCoordinates,
				lightmapperArguments,
				scene.staticGeometry.lightmapAtlasImageAssetDescriptor
			);
		}
	}

	void SceneBuilder::buildEntityComponentMappings(BuildDynamicSceneGeometry3D& dynamicGeometry) {
		dynamicGeometry.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappings.size());
		dynamicGeometry.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(rigidBodyMappings.size());

		std::copy(geometryMappings.begin(), geometryMappings.end(), &dynamicGeometry.geometryMappings[0]);
		std::copy(rigidBodyMappings.begin(), rigidBodyMappings.end(), &dynamicGeometry.rigidBodyMappings[0]);
	}
}