// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/Build.hpp"
#include "AssetBuilder/API/Scene/Builder/EmissiveTriangle.hpp"
#include "AssetBuilder/API/Scene/Builder/GenerateLightmaps.hpp"
#include "AssetBuilder/API/Scene/Builder/PlaceRigidBody.hpp"
#include "AssetBuilder/API/Scene/Builder/PlaceStaticProp.hpp"
#include "AssetBuilder/API/Scene/Builder/PlaceSurface.hpp"
#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/MaterialShader/Lightmapped.hpp"
#include "AssetBuilder/Process/MaterialShader/LightmappedPBR.hpp"
#include "AssetBuilder/Process/MaterialShader/PBR.hpp"
#include "AssetBuilder/Process/MaterialShader/PBRDebug.hpp"

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
				return PBRDebugShaderModuleAsset();
			}
			default: {
				DEBUG_ASSERT(false);

				return {};
			}
		}
	}

	BuildStaticGeometry3D SceneBuilder::createWallGeometry(svec2<float32> scale) {
		const float32 x = scale.x;
		const float32 y = scale.y;

		return {
			.vertices = {
				{{-0.5f * x,  0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
				{{ 0.5f * x,  0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
				{{-0.5f * x, -0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
				{{ 0.5f * x, -0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
			},
			.indices = {
				0, 1, 2,
				1, 3, 2,
			},
			.edgeOriginVertexIndices = {0, 1, 3, 2},
			.normal = {0.0f, 0.0f, -1.0f},
			.scale = scale,
		};
	}

	SceneBuilder::SceneBuilder(const Scene3DDescriptor& descriptor) :
		descriptor(descriptor)
	{
		if (descriptor.skyboxImageAsset != nullptr) {
			ENGINE_ASSERT(descriptor.skyboxImageAsset->isEligibleForCubeMap(), "Skybox image asset is not eligible for a cube map.");
		}
	}

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
			geometry.vertices = sequence<BuildVertex3D>(asset->vertices.count());
			geometry.indices = sequence<uint32>(asset->indices.count());

			uint32 vertexIndex = 0;

			while (vertexIndex < geometry.vertices.count()) {
				const Vertex3D& packedVertex = asset->vertices[vertexIndex];

				BuildVertex3D& vertex = geometry.vertices[vertexIndex];
				vertex.position = packedVertex.position;
				vertex.normal = asset->vertexNormals[vertexIndex];
				vertex.textureCoordinate = unpack<VertexInputAttributeFormat::R16G16_FLOAT, svec2<float32>>(packedVertex.textureCoordinate);

				vertexIndex += 1;
			}

			std::copy(asset->indices.begin(), asset->indices.end(), &geometry.indices[0]);

			geometries.emplace_back(geometry);
		}

		return geometryIndex;
	}

	MaterialReference SceneBuilder::loadMaterial(ref<MaterialAsset> asset) {
		const uint32 materialShaderIndex = findMaterialShaderIndex(asset->shaderModuleAssetIdentifier);

		if (materialShaderIndex >= materialShaders.size()) {
			IntermediateMaterialShader materialShader{};
			materialShader.asset = getMaterialShaderModuleAsset(asset->shaderModuleAssetIdentifier);

			materialShaders.emplace_back(materialShader);
		}

		IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

		const uint32 materialIndex = static_cast<uint32>(materialShader.materials.size());

		IntermediateMaterial material{};
		material.assetIdentifier = asset->identifier;
		material.bakedGeometryIndex = max<uint32>();

		materialShader.materials.emplace_back(material);

		materialCount += 1;

		return (materialShaderIndex << 16) | materialIndex;
	}

	void SceneBuilder::placePointLight(const PointLight& pointLight) {
		pointLights.emplace_back(pointLight);
	}

	uint32 SceneBuilder::findMaterialShaderIndex(uint32 assetIdentifier) const {
		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < materialShaders.size()) {
			const IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

			if (assetIdentifier == materialShader.asset.identifier) {
				break;
			}

			materialShaderIndex += 1;
		}

		return materialShaderIndex;
	}

	uint32 SceneBuilder::findMaterialGeometryIndex(const IntermediateMaterial& material, uint32 geometryIndex) const {
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