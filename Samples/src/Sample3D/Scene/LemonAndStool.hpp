// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	static Scene3D createScene(const AssetLibrary& assetLibrary) {
		Scene3D scene;

		const ref<GeometryAsset> lemonGeometryAsset = assetLibrary.findGeometryAsset("lemon");
		const ref<GeometryAsset> stoolGeometryAsset = assetLibrary.findGeometryAsset("folding_wooden_stool");

		const ref<Geometry3D> lemonGeometry = createRef<Geometry3D>(lemonGeometryAsset);
		const ref<Geometry3D> stoolGeometry = createRef<Geometry3D>(stoolGeometryAsset);

		const uint32 lemonGeometryIndex = scene.registerGeometry(lemonGeometry);
		const uint32 stoolGeometryIndex = scene.registerGeometry(stoolGeometry);

		MaterialPipelineDescriptor defaultMaterialPipelineDescriptor{};
		defaultMaterialPipelineDescriptor.shaderModulePath = "Visibility/Material/Default";
		defaultMaterialPipelineDescriptor.descriptorSetLayoutBindings = {};

		MaterialDescriptor defaultMaterialDescriptor{};
		defaultMaterialDescriptor.pipelineIndex = scene.registerMaterialPipeline(defaultMaterialPipelineDescriptor);
		defaultMaterialDescriptor.writeDescriptorSets = {};

		const uint32 defaultMaterialIndex = scene.registerMaterial(defaultMaterialDescriptor);

		MaterialPipelineDescriptor testMaterialPipelineDescriptor{};
		testMaterialPipelineDescriptor.shaderModulePath = "Visibility/Material/Test";
		testMaterialPipelineDescriptor.descriptorSetLayoutBindings = {};

		MaterialDescriptor testMaterialDescriptor{};
		testMaterialDescriptor.pipelineIndex = scene.registerMaterialPipeline(testMaterialPipelineDescriptor);
		testMaterialDescriptor.writeDescriptorSets = {};

		const uint32 testMaterialIndex = scene.registerMaterial(testMaterialDescriptor);

		InstanceDescriptor lemon1InstanceDescriptor{};
		lemon1InstanceDescriptor.position = {-0.1_m, 0.0_m, 0.3_m};
		lemon1InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		lemon1InstanceDescriptor.geometryIndex = lemonGeometryIndex;
		lemon1InstanceDescriptor.materialIndex = testMaterialIndex;

		InstanceDescriptor lemon2InstanceDescriptor{};
		lemon2InstanceDescriptor.position = {0.0_m, 0.0_m, 0.3_m};
		lemon2InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		lemon2InstanceDescriptor.geometryIndex = lemonGeometryIndex;
		lemon2InstanceDescriptor.materialIndex = testMaterialIndex;

		InstanceDescriptor stoolInstanceDescriptor{};
		stoolInstanceDescriptor.position = {0.5_m, -0.225_m, 1.0_m};
		stoolInstanceDescriptor.rotation = {pi<float32>() * 0.5f, 0.3_rad, 0.0_rad};
		stoolInstanceDescriptor.geometryIndex = stoolGeometryIndex;
		stoolInstanceDescriptor.materialIndex = testMaterialIndex;

		scene.registerInstance(lemon1InstanceDescriptor);
		scene.registerInstance(lemon2InstanceDescriptor);
		scene.registerInstance(stoolInstanceDescriptor);

		return scene;
	}
}