// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	static Scene3D createScene(const AssetLibrary& assetLibrary) {
		Scene3D scene;

		const vec3<float32> size = {1.0_m, 1.0_m, 0.0_m};
		const ref<Geometry3D> wallGeometry = createRef<BoxGeometry3D>(size);
		const uint32 wallGeometryIndex = scene.registerGeometry(wallGeometry);

		InstanceDescriptor wallInstanceDescriptor{};
		wallInstanceDescriptor.position = {0.0_m, 0.0_m, 0.4_m};
		wallInstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		wallInstanceDescriptor.geometryIndex = wallGeometryIndex;
		wallInstanceDescriptor.materialIndex = 1;

		scene.registerInstance(wallInstanceDescriptor);

		return scene;
	}
}