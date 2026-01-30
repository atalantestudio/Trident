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

		InstanceDescriptor lemon1InstanceDescriptor{};
		lemon1InstanceDescriptor.position = {-0.1_m, 0.0_m, 0.3_m};
		lemon1InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		lemon1InstanceDescriptor.geometryIndex = lemonGeometryIndex;
		lemon1InstanceDescriptor.materialIndex = 1;

		InstanceDescriptor lemon2InstanceDescriptor{};
		lemon2InstanceDescriptor.position = {0.0_m, 0.0_m, 0.3_m};
		lemon2InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		lemon2InstanceDescriptor.geometryIndex = lemonGeometryIndex;
		lemon2InstanceDescriptor.materialIndex = 2;

		InstanceDescriptor stoolInstanceDescriptor{};
		stoolInstanceDescriptor.position = {0.5_m, -0.225_m, 1.0_m};
		stoolInstanceDescriptor.rotation = {pi<float32>() * 0.5f, 0.3_rad, 0.0_rad};
		stoolInstanceDescriptor.geometryIndex = stoolGeometryIndex;
		stoolInstanceDescriptor.materialIndex = 3;

		scene.registerInstance(lemon1InstanceDescriptor);
		scene.registerInstance(lemon2InstanceDescriptor);
		scene.registerInstance(stoolInstanceDescriptor);

		return scene;
	}
}