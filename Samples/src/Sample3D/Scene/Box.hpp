// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	static Scene3D createScene(AssetLibrary* assetLibrary) {
		Scene3D scene;

		const vec3<float32> size = {1.0_m, 0.5_m, 0.3_m};
		const ref<Geometry3D> geometry = createRef<BoxGeometry3D>(size);
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
		instanceDescriptor.position = {0.0_m, 0.0_m, 0.4_m};
		instanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		instanceDescriptor.geometryIndex = geometryIndex;
		instanceDescriptor.materialReferenceIndex = pbrMaterialReferenceIndex;

		scene.registerInstance(instanceDescriptor);

		return scene;
	}
}