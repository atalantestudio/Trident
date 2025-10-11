#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace ProjectA {
	static Scene3D createScene(const AssetLibrary& assetLibrary) {
		Scene3D scene;

		const ref<GeometryAsset>& suzanneGeometryAsset = assetLibrary.findGeometryAsset("suzanne");
		const ref<Geometry3D> suzanneGeometry = createRef<Geometry3D>(suzanneGeometryAsset);
		const uint32 suzanneGeometryIndex = scene.registerGeometry(suzanneGeometry);

		InstanceDescriptor suzanneInstanceDescriptor{};
		suzanneInstanceDescriptor.geometryIndex = suzanneGeometryIndex;
		suzanneInstanceDescriptor.position = {0.0_m, 0.0_m, 0.0_m};
		suzanneInstanceDescriptor.rotation = {pi<float32>() * -0.5f, 0.0_rad, pi<float32>()};

		scene.registerInstance(suzanneInstanceDescriptor);

		return scene;
	}
}