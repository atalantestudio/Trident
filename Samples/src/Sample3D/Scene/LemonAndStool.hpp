// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	Scene3D createLemonsAndStoolScene(AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		Scene3D scene;

		const ref<GeometryAsset> lemonGeometryAsset = assetLibrary->getAsset<GeometryAsset>("lemon_1k");
		const ref<GeometryAsset> stoolGeometryAsset = assetLibrary->getAsset<GeometryAsset>("folding_wooden_stool");

		const ref<Geometry3D> lemonGeometry = createRef<Geometry3D>(lemonGeometryAsset);
		const ref<Geometry3D> stoolGeometry = createRef<Geometry3D>(stoolGeometryAsset);

		const uint32 lemonGeometryIndex = scene.registerGeometry(lemonGeometry);
		const uint32 stoolGeometryIndex = scene.registerGeometry(stoolGeometry);

		Material defaultMaterial{};
		defaultMaterial.shaderModulePath = "Visibility/Material/Default";
		defaultMaterial.descriptorSetLayoutBindings = {};

		Material testMaterial{};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				testMaterial.shaderModulePath = "Visibility/Material/Test";

				break;
			}
			case GraphicsAPI::GL: {
				testMaterial.shaderModulePath = "Visibility/Material/TestGL";

				break;
			}
		}

		testMaterial.descriptorSetLayoutBindings = {};

		const uint32 defaultMaterialIndex = scene.registerMaterial(defaultMaterial);
		const uint32 testMaterialIndex = scene.registerMaterial(testMaterial);

		MaterialReference testMaterialReference{};
		testMaterialReference.materialIndex = testMaterialIndex;
		testMaterialReference.descriptorImageAssetPaths = {};

		const uint32 testMaterialReferenceIndex = scene.registerMaterialReference(testMaterialReference);

		SceneInstanceDescriptor lemon1InstanceDescriptor{};
		lemon1InstanceDescriptor.position = {-0.1_m, 0.0_m, 0.3_m};
		lemon1InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		lemon1InstanceDescriptor.geometryIndex = lemonGeometryIndex;
		lemon1InstanceDescriptor.materialReferenceIndex = testMaterialReferenceIndex;

		SceneInstanceDescriptor lemon2InstanceDescriptor{};
		lemon2InstanceDescriptor.position = {0.0_m, 0.0_m, 0.3_m};
		lemon2InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		lemon2InstanceDescriptor.geometryIndex = lemonGeometryIndex;
		lemon2InstanceDescriptor.materialReferenceIndex = testMaterialReferenceIndex;

		SceneInstanceDescriptor stoolInstanceDescriptor{};
		stoolInstanceDescriptor.position = {0.5_m, -0.225_m, 1.0_m};
		stoolInstanceDescriptor.rotation = {pi<float32>() * 0.5f, 0.3_rad, 0.0_rad};
		stoolInstanceDescriptor.geometryIndex = stoolGeometryIndex;
		stoolInstanceDescriptor.materialReferenceIndex = testMaterialReferenceIndex;

		scene.registerInstance(lemon1InstanceDescriptor);
		scene.registerInstance(lemon2InstanceDescriptor);
		scene.registerInstance(stoolInstanceDescriptor);

		return scene;
	}
}