// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident::FlatColor {
	inline Scene3D initializeScene(Application* application, GraphicsAPI graphicsApi, uint64 minStorageBufferOffsetAlignment) {
		AssetLibrary* assetLibrary = application->getAssetLibrary();

		// TODO(asset-library/recursive-asset-loading): Automatically preload the required assets.
		assetLibrary->getAsset<GeometryAsset>("UnitBox");
		assetLibrary->getAsset<GeometryAsset>("RectangularCuboid");

		const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene3DAsset> sceneAsset = assetLibrary->getAsset<Scene3DAsset>(sceneAssetName);

		Scene3D scene(sceneAsset, minStorageBufferOffsetAlignment);

		const MaterialAsset materialAsset = {
			.shaderModuleName = "PBRFlatColor.frag",
			.stage = ShaderStage::FRAGMENT,
			.entryPoint = "main",
			.descriptorBindingTypes = {},
			.descriptorImages = {},
		};
		const uint32 materialShaderIndex = scene.registerMaterialShader();

		scene.registerMaterialAsset(createRef<MaterialAsset>(materialAsset), materialShaderIndex);

		return scene;
	}
}