// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"

namespace Trident {
	class FaceTestScene3D final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_face_test";
			}

			BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MapBuilder builder({
					.skyboxImageAsset = assetLibrary->getAsset<ImageAsset>("evening_road_01_2k"),
					.geometryAssets = {},
					.staticMaterialAssets = {
						assetLibrary->getAsset<MaterialAsset>("lm_default"),
					},
					.dynamicMaterialAssets = {},
				});

				// TODO(asset-builder/map)

				return builder.build();
			}

		public:
			explicit FaceTestScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}