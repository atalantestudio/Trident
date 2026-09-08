// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Image/CubeMapProcess.hpp"

namespace Trident {
	class OvercastSoilProcess final : public CubeMapProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "overcast_soil_512p";
			}

			view<view<char8>> getFacePaths() const final override {
				static constexpr view<char8> paths[] = {
					"overcast_soil_512p/front.png",
					"overcast_soil_512p/back.png",
					"overcast_soil_512p/up.png",
					"overcast_soil_512p/down.png",
					"overcast_soil_512p/right.png",
					"overcast_soil_512p/left.png",
				};

				return paths;
			}

		public:
			explicit OvercastSoilProcess(AssetBuilderLibrary* assetLibrary) :
				CubeMapProcess(assetLibrary)
			{}
	};
}