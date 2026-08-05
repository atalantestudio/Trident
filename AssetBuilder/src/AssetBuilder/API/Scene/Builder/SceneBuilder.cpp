// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/Build.hpp"
#include "AssetBuilder/API/Scene/Builder/CreateLightmapperGeometryBuffers.hpp"
#include "AssetBuilder/API/Scene/Builder/GenerateLightmaps.hpp"
#include "AssetBuilder/API/Scene/Builder/PlaceRigidBody.hpp"
#include "AssetBuilder/API/Scene/Builder/PlaceStaticProp.hpp"
#include "AssetBuilder/API/Scene/Builder/PlaceSurface.hpp"
#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/MaterialShader/Lightmapped.hpp"
#include "AssetBuilder/Process/MaterialShader/LightmappedPBR.hpp"
#include "AssetBuilder/Process/MaterialShader/PBR.hpp"
#include "AssetBuilder/Process/MaterialShader/PBRSeparatedAORoughness.hpp"

namespace Trident {
	Lightmapper SceneBuilder::lightmapper;

	ShaderModuleAsset SceneBuilder::getMaterialShaderModuleAsset(uint32 identifier) {
		switch (identifier) {
			case 400: {
				return LightmappedShaderModuleAsset();
			}
			case 401: {
				return LightmappedPBRShaderModuleAsset();
			}
			case 402: {
				return PBRShaderModuleAsset();
			}
			case 403: {
				return PBRSeparatedAORoughnessShaderModuleAsset();
			}
			default: {
				DEBUG_ASSERT(false);

				return {};
			}
		}
	}

	StaticGeometry3D SceneBuilder::createWallGeometry(svec2<float32> scale) {
		return {
			.vertexComponents = {
				-0.5f * scale.x,  0.5f * scale.y,  0.0f,
				 0.5f * scale.x,  0.5f * scale.y,  0.0f,
				-0.5f * scale.x, -0.5f * scale.y,  0.0f,
				 0.5f * scale.x, -0.5f * scale.y,  0.0f,
			},
			.vertexTextureCoordinateComponents = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f,
			},
			.vertexIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.vertexTextureCoordinateIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.edgeOriginVertexIndices = {0, 1, 3, 2},
			.normal = {0.0f, 0.0f, -1.0f},
			.scale = scale,
		};
	}

	void SceneBuilder::release() {
		lightmapper.~Lightmapper();
	}

	SceneBuilder::SceneBuilder(const LightmapperArguments& lightmapperArguments) :
		lightmapperArguments(lightmapperArguments)
	{}

	uint32 SceneBuilder::loadGeometry(ref<Geometry3DAsset> asset) {
		uint32 geometryIndex = 0;

		while (geometryIndex < geometries.size()) {
			const IntermediateGeometry& geometry = geometries[geometryIndex];

			if (asset->identifier == geometry.assetIdentifier) {
				break;
			}

			geometryIndex += 1;
		}

		if (geometryIndex >= geometries.size()) {
			IntermediateGeometry geometry{};
			geometry.assetIdentifier = asset->identifier;
			geometry.sharedInstanceCount = 0;
			geometry.vertices = sequence<GeometryIndex3D>(asset->indexCount);
			geometry.positions = sequence<svec3<float32>>(asset->positions.count());
			geometry.normals = sequence<svec3<float32>>(asset->normals.count());
			geometry.textureCoordinates = sequence<svec2<float32>>(asset->textureCoordinates.count());

			uint32 vertexIndex = 0;

			while (vertexIndex < geometry.vertices.count()) {
				GeometryIndex3D& vertex = geometry.vertices[vertexIndex];
				vertex.positionIndex = asset->positionIndices[vertexIndex];
				vertex.normalIndex = asset->normalIndices[vertexIndex];
				vertex.textureCoordinateIndex = asset->textureCoordinateIndices[vertexIndex];

				vertexIndex += 1;
			}

			std::copy_n(&asset->positions[0], geometry.positions.count(), &geometry.positions[0]);
			std::copy_n(&asset->normals[0], geometry.normals.count(), &geometry.normals[0]);
			std::copy_n(&asset->textureCoordinates[0], geometry.textureCoordinates.count(), &geometry.textureCoordinates[0]);

			geometry.transformedTextureCoordinates = geometry.textureCoordinates;

			geometries.emplace_back(geometry);
		}

		return geometryIndex;
	}

	MaterialReference SceneBuilder::loadStaticMaterial(ref<MaterialAsset> asset) {
		const uint32 geometryIndex = static_cast<uint32>(geometries.size());

		IntermediateGeometry geometry{};
		geometry.assetIdentifier = 0;
		geometry.sharedInstanceCount = 0;

		geometries.emplace_back(geometry);

		const MaterialReference materialReference = loadMaterial(asset, staticMaterialShaders);

		IntermediateMaterial& material = getMaterial(materialReference);

		IntermediateMaterialGeometry materialGeometry{};
		materialGeometry.geometryIndex = geometryIndex;

		material.geometries.emplace_back(materialGeometry);

		materialGeometryCount += 1;

		return materialReference;
	}

	MaterialReference SceneBuilder::loadDynamicMaterial(ref<MaterialAsset> asset) {
		return loadMaterial(asset, dynamicMaterialShaders);
	}

	IntermediateMaterial& SceneBuilder::getMaterial(MaterialReference materialReference) {
		const uint32 materialShaderIndex = materialReference >> 16;
		const uint32 materialIndex = materialReference & 0xff;

		return staticMaterialShaders[materialShaderIndex].materials[materialIndex];
	}

	MaterialReference SceneBuilder::loadMaterial(ref<MaterialAsset> asset, std::vector<IntermediateMaterialShader>& materialShaders) {
		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < materialShaders.size()) {
			const IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

			if (asset->shaderModuleAssetIdentifier == materialShader.assetIdentifier) {
				break;
			}

			materialShaderIndex += 1;
		}

		if (materialShaderIndex >= materialShaders.size()) {
			IntermediateMaterialShader materialShader{};
			materialShader.assetIdentifier = asset->shaderModuleAssetIdentifier;

			materialShaders.emplace_back(materialShader);
		}

		IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

		const uint32 materialIndex = static_cast<uint32>(materialShader.materials.size());

		IntermediateMaterial material{};
		material.assetIdentifier = asset->identifier;

		materialShader.materials.emplace_back(material);

		materialCount += 1;

		return (materialShaderIndex << 16) | materialIndex;
	}

	uint32 SceneBuilder::findMaterialGeometryIndex(const IntermediateMaterial& material, uint32 geometryIndex) {
		uint32 materialGeometryIndex = 0;

		while (materialGeometryIndex < material.geometries.size()) {
			const IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];

			if (geometryIndex == materialGeometry.geometryIndex) {
				break;
			}

			materialGeometryIndex += 1;
		}

		return materialGeometryIndex;
	}
}