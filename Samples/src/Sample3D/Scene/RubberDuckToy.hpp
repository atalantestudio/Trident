// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	static Scene3D createScene(const AssetLibrary& assetLibrary) {
		Scene3D scene;

		const ref<GeometryAsset> geometryAsset = assetLibrary.findGeometryAsset("rubber_duck_toy_4k");
		const ref<Geometry3D> geometry = createRef<Geometry3D>(geometryAsset);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		InstanceDescriptor instanceDescriptor{};
		instanceDescriptor.position = {0.0_m, -0.125_m, 0.0_m};
		instanceDescriptor.rotation = {pi<float32>() * 0.5f, -pi<float32>() * 0.5f, 0.0_rad};
		instanceDescriptor.geometryIndex = geometryIndex;
		instanceDescriptor.materialIndex = 1;

		scene.registerInstance(instanceDescriptor);

		return scene;
	}
}