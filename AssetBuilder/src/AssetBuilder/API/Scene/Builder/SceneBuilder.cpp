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

	BuildStaticGeometry3D SceneBuilder::createWallGeometry(svec2<float32> scale) {
		const float32 x = scale.x;
		const float32 y = scale.y;

		return {
			.vertices = {
				{{-0.5f * x,  0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
				{{ 0.5f * x,  0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
				{{-0.5f * x, -0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
				{{ 0.5f * x, -0.5f * y,  0.0f}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
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

	void SceneBuilder::release() {
		lightmapper.~Lightmapper();
	}

	SceneBuilder::SceneBuilder(const SceneLightingDescriptor& lighting) :
		lighting(lighting)
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
			geometry.vertices = sequence<BuildVertex3D>(asset->vertices.count());
			geometry.indices = sequence<uint32>(asset->indices.count());

			const uint32 vertexCount = static_cast<uint32>(asset->vertices.count());

			uint32 vertexIndex = 0;

			while (vertexIndex < vertexCount) {
				const Vertex3D& packedVertex = asset->vertices[vertexIndex];

				BuildVertex3D& vertex = geometry.vertices[vertexIndex];
				vertex.position = packedVertex.position;

				// TODO(lyah/scalar)
				vertex.normal = unpack<VertexInputAttributeFormat::R10G10B10A2_UNORM, svec3<float32>>(packedVertex.normal) * 2.0f - svec3<float32>{1.0f, 1.0f, 1.0f};
				vertex.tangent = unpack<VertexInputAttributeFormat::R10G10B10A2_UNORM, svec3<float32>>(packedVertex.tangent) * 2.0f - svec3<float32>{1.0f, 1.0f, 1.0f};
				vertex.textureCoordinate = unpack<VertexInputAttributeFormat::R16G16_UNORM, svec2<float32>>(packedVertex.textureCoordinate);

				vertexIndex += 1;
			}

			std::copy(asset->indices.begin(), asset->indices.end(), &geometry.indices[0]);

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