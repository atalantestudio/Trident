// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	static Scene3D createScene(const AssetLibrary& assetLibrary) {
		Scene3D scene;

		Geometry3DDescriptor geometryDescriptor{};
		geometryDescriptor.vertexPositionComponents = {
			-0.5f,  0.5f,  0.0f,
			 0.5f,  0.5f,  0.0f,
			 0.5f, -0.5f,  0.0f,
			-0.5f, -0.5f,  0.0f,
		};
		geometryDescriptor.vertexPositionIndices = {
			0, 1, 2,
			0, 2, 3,
		};
		geometryDescriptor.vertexNormalComponents = {
			0.0f,  0.0f, -1.0f,
		};
		geometryDescriptor.vertexNormalIndices = {
			0, 0, 0,
			0, 0, 0,
		};
		geometryDescriptor.vertexTextureCoordinateComponents = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
		};
		geometryDescriptor.vertexTextureCoordinateIndices = {
			0, 1, 2,
			0, 2, 3,
		};

		const ref<Geometry3D> geometry = createRef<Geometry3D>(geometryDescriptor);
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