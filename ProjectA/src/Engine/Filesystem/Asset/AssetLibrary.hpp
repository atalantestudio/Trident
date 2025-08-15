#pragma once

#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace ProjectA {
	class AssetLibrary {
		public:
			static uint32 createAssetIdentifier() {
				return random<uint32>(min<uint32>(), max<uint32>());
			}

		public:
			bool isLoaded(uint32 assetIdentifier) const {
				for (uint32 assetIndex = 0; assetIndex < fontAssets.size(); assetIndex += 1) {
					if (fontAssets[assetIndex].identifier == assetIdentifier) {
						return true;
					}
				}

				for (uint32 assetIndex = 0; assetIndex < imageAssets.size(); assetIndex += 1) {
					if (imageAssets[assetIndex].identifier == assetIdentifier) {
						return true;
					}
				}

				return false;
			}

			// TODO: Implement.
			// void load(std::string_view path);

		private:
			std::vector<FontAsset> fontAssets;
			std::vector<ImageAsset> imageAssets;
	};
}