// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class LemonsScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_lemons";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MapBuilder builder({
					.skyColor = {0.6f, 0.7f, 0.9f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.viewDirection = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 lemonGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("lemon"));

				const MaterialReference lemonMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lemon_1k"));

				builder.createRigidBody({
					.geometryIndex = lemonGeometry,
					.material = lemonMaterial,
					.position = {0.0f, 64.0f, 128.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 16.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.2f,
					.restitution = 0.0f,
				});

				builder.createPointLight({
					.color = {1.0f, 1.0f, 1.0f},
					.intensity = 300000.0f,
					.position = {-256.0f, 320.0f, 0.0f},
				});

				builder.createPointLight({
					.color = {1.0f, 1.0f, 1.0f},
					.intensity = 300000.0f,
					.position = {256.0f, -192.0f, 0.0f},
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}