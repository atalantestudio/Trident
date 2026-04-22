// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::FlatColor {
	inline Scene3D initializeScene(Application* application, GraphicsAPI graphicsApi, uint64 minStorageBufferOffsetAlignment) {
		AssetLibrary* assetLibrary = application->getAssetLibrary();

		// Preload geometry assets.
		assetLibrary->getAsset<GeometryAsset>("UnitBox");

		const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene3DAsset> sceneAsset = assetLibrary->getAsset<Scene3DAsset>(sceneAssetName);

		Scene3D scene(sceneAsset, minStorageBufferOffsetAlignment);
		scene.registerMaterial({
			.shaderIndex = scene.registerMaterialShader({
				.modulePath = "Material/PBRFlatColor.frag",
				.descriptorSetLayoutBindings = {},
			}),
			.writeDescriptorSetBindings = {},
		});

		return scene;
	}
}