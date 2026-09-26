// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	MaterialReference MapBuilder::loadMaterial(ref<MaterialAsset> asset) {
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
}