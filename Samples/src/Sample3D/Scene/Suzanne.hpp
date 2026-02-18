// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	Scene3D createSuzanneScene(AssetLibrary* assetLibrary) {
		Scene3D scene;

		const ref<GeometryAsset> geometryAsset = assetLibrary->getAsset<GeometryAsset>("suzanne");
		const ref<Geometry3D> geometry = createRef<Geometry3D>(geometryAsset);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		Material pbrMaterial{};
		pbrMaterial.shaderModulePath = "Material/PBR.frag";
		pbrMaterial.descriptorSetLayoutBindings = {};

		const uint32 pbrMaterialIndex = scene.registerMaterial(pbrMaterial);

		MaterialReference pbrMaterialReference{};
		pbrMaterialReference.materialIndex = pbrMaterialIndex;
		pbrMaterialReference.descriptorImagePaths = {};

		const uint32 pbrMaterialReferenceIndex = scene.registerMaterialReference(pbrMaterialReference);

		InstanceDescriptor instanceDescriptor{};
		instanceDescriptor.position = {0.0_m, 0.0_m, 0.0_m};
		instanceDescriptor.rotation = {pi<float32>() * -0.5f, 0.0_rad, pi<float32>()};
		instanceDescriptor.geometryIndex = geometryIndex;
		instanceDescriptor.materialReferenceIndex = pbrMaterialReferenceIndex;

		scene.registerInstance(instanceDescriptor);

		return scene;
	}
}