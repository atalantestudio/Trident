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

				const FaceVertex pentagonVertices[] = {
					{{-32.0f,  32.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 0.0f,   32.0f,  112.0f}, {0.0f, 0.0f}},
					{{ 32.0f,  32.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 16.0f,  32.0f,  64.0f }, {0.0f, 0.0f}},
					{{-16.0f,  32.0f,  64.0f }, {0.0f, 0.0f}},
				};

				Face& pentagon = builder.createFace(pentagonVertices, {0.0f, 1.0f, 0.0f});

				builder.extrudeFace(pentagon, 32.0f);

				const FaceVertex squareVertices[] = {
					{{ 64.0f,  48.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 96.0f,  48.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 96.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 64.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
				};

				Face& square = builder.createFace(squareVertices, {0.0f, 0.0f, -1.0f});

				builder.extrudeFace(square, -32.0f);

				const FaceVertex roomVertices[] = {
					{{-64.0f,  128.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 64.0f,  128.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 64.0f,  128.0f, -128.0f}, {0.0f, 0.0f}},
					{{-64.0f,  128.0f, -128.0f}, {0.0f, 0.0f}},
				};

				Face& room = builder.createFace(roomVertices, {0.0f, 1.0f, 0.0f});

				builder.extrudeFace(room, -128.0f);

				const FaceVertex concaveFaceVertices[] = {
					{{-48.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  128.0f}, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  112.0f}, {0.0f, 0.0f}},
					{{ 16.0f,  24.0f,  112.0f}, {0.0f, 0.0f}},
					{{ 16.0f,  24.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  92.0f }, {0.0f, 0.0f}},
					{{ 48.0f,  24.0f,  64.0f }, {0.0f, 0.0f}},
					{{-48.0f,  24.0f,  64.0f }, {0.0f, 0.0f}},
				};

				Face& concaveFace = builder.createFace(concaveFaceVertices, {0.0f, 1.0f, 0.0f});

				return builder.build();
			}

		public:
			explicit FaceTestScene3D(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}