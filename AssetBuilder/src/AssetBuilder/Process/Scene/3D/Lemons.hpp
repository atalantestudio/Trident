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
					.skyboxImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_2k"),
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});

				const uint32 lemonGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("lemon"));

				const MaterialReference lemonMaterial = builder.loadDynamicMaterial(assetLibrary->getAsset<MaterialAsset>("lemon_1k"));

				builder.placeRigidBody({
					.geometryIndex = lemonGeometry,
					.material = lemonMaterial,
					.position = {3.84f, 0.0f, 16.0f},
					.rotation = {radians(90.0_deg), -0.2_rad, radians(180.0_deg)},
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
					.position = {-4.48f, -2.56f, 16.0f},
					.rotation = {-0.3_rad, -0.3_rad, radians(90.0_deg)},
					.uniformScale = 128.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				return builder.build();
			}

		public:
			explicit LemonsScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}