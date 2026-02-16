// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	static Scene3D createScene(AssetLibrary* assetLibrary) {
		Scene3D scene;

		const vec3<float32> size = {1.0_m, 1.0_m, 1.0_m};
		const ref<Geometry3D> geometry = createRef<BoxGeometry3D>(size);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		Material defaultMaterial{};
		defaultMaterial.shaderModulePath = "Visibility/Material/Default";
		defaultMaterial.descriptorSetLayoutBindings = {};

		Material testMaterial{};
		testMaterial.shaderModulePath = "Visibility/Material/Test";
		testMaterial.descriptorSetLayoutBindings = {};

		const uint32 defaultMaterialIndex = scene.registerMaterial(defaultMaterial);
		const uint32 testMaterialIndex = scene.registerMaterial(testMaterial);

		MaterialReference testMaterialReference{};
		testMaterialReference.materialIndex = testMaterialIndex;
		testMaterialReference.descriptorImageAssetPaths = {};

		const uint32 testMaterialReferenceIndex = scene.registerMaterialReference(testMaterialReference);

		InstanceDescriptor instanceDescriptor{};
		instanceDescriptor.position = {0.0_m, 0.0_m, 0.0_m};
		instanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		instanceDescriptor.geometryIndex = geometryIndex;
		instanceDescriptor.materialReferenceIndex = testMaterialReferenceIndex;

		scene.registerInstance(instanceDescriptor);

		return scene;
	}
}