// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class PropGalleryScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_prop_gallery";
			}

			MapBuilder createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MapBuilder builder({
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

				const uint32 powerBox01Geometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("power_box_01"));
				const uint32 utilityBox01Geometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("utility_box_01"));
				const uint32 utilityBox02Geometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("utility_box_02"));
				const uint32 wetFloorSign01Geometry = builder.loadGeometry(assetLibrary->getAsset<Geometry3DAsset>("wet_floor_sign_01"));

				const MaterialReference powerBox01Material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lpbr_power_box_01_1k"));
				const MaterialReference utilityBox01Material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lpbr_utility_box_01_1k"));
				const MaterialReference utilityBox02Material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lpbr_utility_box_02_1k"));
				const MaterialReference wetFloorSign01Material = builder.loadMaterial(assetLibrary->getAsset<MaterialAsset>("lpbr_wet_floor_sign_01_1k"));

				builder.createStaticProp({
					.geometryIndex = powerBox01Geometry,
					.material = powerBox01Material,
					.transform = {
						.translation = {-128.0f, 0.0f, 128.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 1.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {64.0f, 64.0f},
				});

				builder.createStaticProp({
					.geometryIndex = utilityBox01Geometry,
					.material = utilityBox01Material,
					.transform = {
						.translation = {-64.0f, 0.0f, 128.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 1.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {64.0f, 64.0f},
				});

				builder.createStaticProp({
					.geometryIndex = utilityBox02Geometry,
					.material = utilityBox02Material,
					.transform = {
						.translation = {64.0f, 0.0f, 128.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 1.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {64.0f, 64.0f},
				});

				builder.createStaticProp({
					.geometryIndex = wetFloorSign01Geometry,
					.material = wetFloorSign01Material,
					.transform = {
						.translation = {128.0f, 0.0f, 128.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = 1.0f,
					},
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.albedo = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {64.0f, 64.0f},
				});

				return builder;
			}

		public:
			using Scene3DProcess::Scene3DProcess;
	};
}