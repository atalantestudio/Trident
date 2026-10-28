// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class DragonScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_dragon";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr vec3<float32> FLOOR_VERTICES[] = {
					{-256.0f,  0.0f,  384.0f},
					{ 256.0f,  0.0f,  384.0f},
					{ 256.0f,  0.0f, -128.0f},
					{-256.0f,  0.0f, -128.0f},
				};

				MapBuilder builder({
					.skyColor = {0.6f, 0.7f, 0.9f},
					.environmentMapImageAsset = assetLibrary->getAsset<ImageAsset>("overcast_soil_512p"),
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.viewDirection = {0.0f, 0.0f, 1.0f},
					},
				});

				const uint32 dragonGeometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("stanford_dragon_low"));

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

				builder.createFace({
					.vertices = FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = material,
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {1.0f, 1.0f},
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});

				builder.createStaticProp({
					.geometryIndex = dragonGeometry,
					.material = material,
					.transform = {
						.translation = {0.0f, 45.5f, 128.0f},
						.rotation = {0.0_rad, radians(-90.0_deg), radians(90.0_deg)},
						.scale = 0.5f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {500.0f, 500.0f},
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}