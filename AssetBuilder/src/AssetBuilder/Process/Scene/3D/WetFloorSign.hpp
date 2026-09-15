// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class WetFloorSignScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_wet_floor_sign";
			}

			SceneBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = {},
					.skyColor = {0.0f, 0.0f, 0.0f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("overcast_soil_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 geometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("wet_floor_sign_01"));

				const MaterialReference lpbrMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lpbr_wet_floor_sign_01_1k"));
				const MaterialReference pbrMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("pbr_default"));

				builder.placeStaticProp({
					.geometryIndex = geometry,
					.material = lpbrMaterial,
					.transform = {
						.translation = {-32.0f, 32.0f, 64.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 1.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {64.0f, 64.0f},
				});

				builder.placeRigidBody({
					.geometryIndex = geometry,
					.material = pbrMaterial,
					.position = {32.0f, 32.0f, 64.0f},
					.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
					.uniformScale = 1.0f,
					.mass = max<float32>(),
					.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
					.inertiaScale = {1.0f, 1.0f, 1.0f},
					.friction = 0.0f,
					.restitution = 0.0f,
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}