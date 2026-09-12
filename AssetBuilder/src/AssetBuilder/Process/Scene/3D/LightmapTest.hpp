// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class LightmapTestScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_lightmap_test";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				SceneBuilder builder({
					.flags = {},
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

				// Wall
				builder.placeSurface(
					{
						.geometry = {
							.vertices = {
								{{-64.0f,  64.0f,  128.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
								{{ 66.0f,  64.0f,  128.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
								{{-64.0f, -64.0f,  128.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
								{{ 66.0f, -64.0f,  128.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
							},
							.indices = {
								0, 1, 2,
								1, 3, 2,
							},
							.edgeOriginVertexIndices = {0, 1, 3, 2},
							.normal = { 0.0f,  0.0f, -1.0f},
							.scale = {130.0f, 128.0f},
						},
						.material = material,
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.lightmap = {
							.luxelScale = {16.0f, 16.0f},
							.albedo = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {0.0f, 0.0f, 0.0f},
						.axis = {0.0f, 0.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				// Small light
				builder.placeSurface(
					{
						.geometry = {
							.vertices = {
								{{ 8.0f,  8.0f, -16.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
								{{-8.0f,  8.0f, -16.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
								{{ 8.0f, -8.0f, -16.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
								{{-8.0f, -8.0f, -16.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
							},
							.indices = {
								0, 1, 2,
								1, 3, 2,
							},
							.edgeOriginVertexIndices = {0, 1, 3, 2},
							.normal = { 0.0f,  0.0f,  1.0f},
							.scale = {16.0f, 16.0f},
						},
						.material = material,
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.lightmap = {
							.luxelScale = {16.0f, 16.0f},
							.emissiveColor = {4.0f, 4.0f, 4.0f},
							.emissiveIntensity = 1.0f,
							.albedo = {1.0f, 1.0f, 1.0f},
						},
					},
					{
						.translation = {0.0f, 0.0f, 0.0f},
						.axis = {0.0f, 0.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				return builder.build();
			}

		public:
			explicit LightmapTestScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}