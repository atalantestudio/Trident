// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class VCGIScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_vcgi";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr vec3<float32> FLOOR_VERTICES[] = {
					{-128.0f,  0.0f,  128.0f},
					{ 128.0f,  0.0f,  128.0f},
					{ 128.0f,  0.0f, -128.0f},
					{-128.0f,  0.0f, -128.0f},
				};
				static constexpr vec3<float32> GLASS_VERTICES[] = {
					{ 0.0f,  48.0f, -32.0f},
					{ 0.0f,  48.0f,  32.0f},
					{ 0.0f,  0.0f,   32.0f},
					{ 0.0f,  0.0f,  -32.0f},
				};

				MapBuilder builder({
					.skyColor = {0.1f, 0.1f, 0.1f},
					.environmentMapImageAsset = nullptr,
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
					.spawn = {
						.position = {8.0f, 0.0f, -256.0f},
						.viewDirection = {0.0f, 0.0f, 1.0f},
					},
				});

				const MaterialReference tileFloorMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_tile_floor"));
				const MaterialReference debugOrangeMaterial = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_debug_orange"));

				builder.createFace({
					.vertices = FLOOR_VERTICES,
					.normal = {0.0f, 1.0f, 0.0f},
					.material = tileFloorMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {64.0f, 64.0f},
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});
				builder.createFace({
					.vertices = GLASS_VERTICES,
					.normal = {1.0f, 0.0f, 0.0f},
					.material = debugOrangeMaterial,
					.textureFiltering = {
						.translation = {0.0f, 0.0f},
						.rotation = 0.0f,
						.scale = {128.0f, 128.0f},
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});

				builder.createPointLight({
					.color = {1.0f, 1.0f, 1.0f},
					.intensity = 300.0f,
					.position = {-128.0f, 128.0f, 0.0f},
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}