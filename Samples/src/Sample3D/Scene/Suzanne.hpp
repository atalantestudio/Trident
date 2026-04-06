// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	Scene3D createSuzanneScene(AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		Scene3D scene;

		const ref<GeometryAsset> geometryAsset = assetLibrary->getAsset<GeometryAsset>("suzanne");
		const ref<Geometry3D> geometry = createRef<Geometry3D>(geometryAsset);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		Material pbrMaterial{};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterial.shaderModulePath = "Material/PBR.frag";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterial.shaderModulePath = "Material/PBRSingleSet.frag";

				break;
			}
		}

		pbrMaterial.descriptorSetLayoutBindings = {};

		const uint32 pbrMaterialIndex = scene.registerMaterial(pbrMaterial);

		MaterialReference pbrMaterialReference{};
		pbrMaterialReference.materialIndex = pbrMaterialIndex;
		pbrMaterialReference.descriptorImagePaths = {};

		const uint32 pbrMaterialReferenceIndex = scene.registerMaterialReference(pbrMaterialReference);

		/* SceneInstanceDescriptor instanceDescriptor{};
		instanceDescriptor.position = {0.0_m, 0.0_m, 0.0_m};
		instanceDescriptor.rotation = {pi<float32>() * -0.5f, 0.0_rad, pi<float32>()};
		instanceDescriptor.geometryIndex = geometryIndex;
		instanceDescriptor.materialReferenceIndex = pbrMaterialReferenceIndex;

		scene.registerInstance(instanceDescriptor); */

		return scene;
	}
}