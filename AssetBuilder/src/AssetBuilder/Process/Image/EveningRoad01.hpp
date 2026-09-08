// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Image/CubeMapProcess.hpp"

namespace Trident {
	class EveningRoad01Process final : public CubeMapProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "evening_road_01_512p";
			}

			view<view<char8>> getFacePaths() const final override {
				static constexpr view<char8> paths[] = {
					"evening_road_01_512p/front.png",
					"evening_road_01_512p/back.png",
					"evening_road_01_512p/up.png",
					"evening_road_01_512p/down.png",
					"evening_road_01_512p/right.png",
					"evening_road_01_512p/left.png",
				};

				return paths;
			}

		public:
			explicit EveningRoad01Process(AssetBuilderLibrary* assetLibrary) :
				CubeMapProcess(assetLibrary)
			{}
	};
}