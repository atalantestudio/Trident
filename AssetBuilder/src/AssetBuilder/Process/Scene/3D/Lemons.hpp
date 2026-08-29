// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class LemonsScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_lemons";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = {},
					.skyColor = {0.6f, 0.7f, 0.9f},
					.skyboxImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_2k"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 lemonGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("lemon"));

				const MaterialReference lemonMaterial = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("lemon_1k"));

				builder.placeRigidBody({
					.geometryIndex = lemonGeometry,
					.material = lemonMaterial,
					.position = {3.97f, 64.0f, 16.0f},
					.rotation = {radians(-45.0_deg), radians(83.0_deg), radians(63.0_deg)},
					.uniformScale = 128.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				builder.placeRigidBody({
					.geometryIndex = lemonGeometry,
					.material = lemonMaterial,
					.position = {-4.63f, 61.44f, 16.0f},
					.rotation = {-0.3_rad, -0.3_rad, radians(90.0_deg)},
					.uniformScale = 128.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				builder.placePointLight({
					.color = {1.0f, 1.0f, 1.0f},
					.intensity = 300000.0f,
					.position = {-256.0f, 256.0f, -256.0f},
				});
				builder.placePointLight({
					.color = {1.0f, 1.0f, 1.0f},
					.intensity = 300000.0f,
					.position = {128.0f, 128.0f, 256.0f},
				});

				return builder.build();
			}

		public:
			explicit LemonsScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}