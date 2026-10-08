// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class LightmapTestScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_lightmap_test";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				static constexpr vec3<float32> WALL_VERTICES[] = {
					{-64.0f,  64.0f,  128.0f},
					{ 66.0f,  64.0f,  128.0f},
					{ 66.0f, -64.0f,  128.0f},
					{-64.0f, -64.0f,  128.0f},
				};
				static constexpr vec3<float32> LIGHT_VERTICES[] = {
					{ 8.0f,  8.0f, -16.0f},
					{-8.0f,  8.0f, -16.0f},
					{-8.0f, -8.0f, -16.0f},
					{ 8.0f, -8.0f, -16.0f},
				};
				static constexpr TextureFilteringDescriptor TEXTURE_FILTERING = {
					.translation = {0.0f, 0.0f},
					.rotation = 0.0f,
					.scale = {1.0f, 1.0f},
				};

				MapBuilder builder({
					.skyColor = {0.05f, 0.05f, 0.05f},
					.environmentMapImageAsset = nullptr,
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {0.0f, 0.0f, 0.0f},
					.sunIntensity = 0.0f,
					.spawn = {
						.position = {0.0f, 0.0f, 0.0f},
						.direction = {0.0f, 0.0f, 1.0f},
					},
				});

				const MaterialReference material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lm_default"));

				builder.createFace({
					.vertices = WALL_VERTICES,
					.normal = {0.0f, 0.0f, -1.0f},
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = {16.0f, 16.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});
				builder.createFace({
					.vertices = LIGHT_VERTICES,
					.normal = {0.0f, 0.0f, 1.0f},
					.material = material,
					.textureFiltering = TEXTURE_FILTERING,
					.lightmap = {
						.luxelScale = {16.0f, 16.0f},
						.emissiveColor = {4.0f, 4.0f, 4.0f},
						.emissiveIntensity = 1.0f,
						.albedo = {1.0f, 1.0f, 1.0f},
					},
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}