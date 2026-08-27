// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class PlayerMovementScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_player_movement";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = {},
					.skyboxImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_2k"),
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});

				const uint32 boxGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("3d_unit_box_st"));

				const MaterialReference debugMaterial = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("debug"));

				/* // Floor
				builder.placeRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {0.0_m, -0.05_m, 0.0_m},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {10.0f, 0.1f, 10.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Box
				builder.placeRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {1.0_m, 0.5_m, 3.0_m},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Player
				builder.placeRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {0.0_m, 2.0_m, 0.0_m},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = 70.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {0.7f, 1.8f, 0.7f},
					.friction = 0.2f,
					.restitution = 0.0f,
				}); */

				// Floor
				builder.placeRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {0.0f, -8.0f, 0.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {512.0f, 16.0f, 512.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Box
				builder.placeRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {128.0f, 8.0f, 128.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = 20.0_kg,
					// .mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {64.0f, 16.0f, 64.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				// Player
				builder.placeRigidBody({
					.geometryIndex = boxGeometry,
					.material = debugMaterial,
					.position = {0.0f, 128.0f, 0.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = 70.0_kg,
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {33.0f, 77.0f, 33.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				return builder.build();
			}

		public:
			explicit PlayerMovementScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}