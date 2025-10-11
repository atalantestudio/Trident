#pragma once

#include "Engine/API/Scene/Scene3D.hpp"

namespace ProjectA {
	static Scene3D createScene(const AssetLibrary& assetLibrary) {
		Scene3D scene;

		const ref<GeometryAsset>& shelfGeometryAsset = assetLibrary.findGeometryAsset("shelf");
		const ref<GeometryAsset>& tableGeometryAsset = assetLibrary.findGeometryAsset("wooden_table_02");

		const ref<Geometry3D> shelfGeometry = createRef<Geometry3D>(shelfGeometryAsset);
		const ref<Geometry3D> tableGeometry = createRef<Geometry3D>(tableGeometryAsset);

		const uint32 shelfGeometryIndex = scene.registerGeometry(shelfGeometry);
		const uint32 tableGeometryIndex = scene.registerGeometry(tableGeometry);

		InstanceDescriptor shelf1InstanceDescriptor{};
		shelf1InstanceDescriptor.geometryIndex = shelfGeometryIndex;
		shelf1InstanceDescriptor.position = {-2.0_m, -1.0_m, 3.0_m};
		shelf1InstanceDescriptor.rotation = {pi<float32>() * 0.5f, 0.0_rad, 0.0_rad};

		InstanceDescriptor woodenTable02InstanceDescriptor{};
		woodenTable02InstanceDescriptor.geometryIndex = tableGeometryIndex;
		woodenTable02InstanceDescriptor.position = {0.0_m, -1.0_m, 3.0_m};
		woodenTable02InstanceDescriptor.rotation = {pi<float32>() * 0.5f, pi<float32>(), 0.0_rad};

		InstanceDescriptor shelf2InstanceDescriptor{};
		shelf2InstanceDescriptor.geometryIndex = shelfGeometryIndex;
		shelf2InstanceDescriptor.position = {2.0_m, -1.0_m, 3.0_m};
		shelf2InstanceDescriptor.rotation = {pi<float32>() * 0.5f, pi<float32>(), 0.0_rad};

		scene.registerInstance(shelf1InstanceDescriptor);
		scene.registerInstance(woodenTable02InstanceDescriptor);
		scene.registerInstance(shelf2InstanceDescriptor);

		return scene;
	}
}