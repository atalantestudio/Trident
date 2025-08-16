#pragma once

#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/AssetSignature.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	class AssetLibrary {
		public:
			static uint32 createAssetIdentifier() {
				return random<uint32>(min<uint32>(), max<uint32>());
			}

		public:
			bool isLoaded(uint32 assetIdentifier) const {
				for (uint32 assetIndex = 0; assetIndex < assets.size(); assetIndex += 1) {
					if (assets[assetIndex]->identifier == assetIdentifier) {
						return true;
					}
				}

				return false;
			}

			void loadAssets() {
				const std::string assetDirectory = std::filesystem::current_path().string() + "/Asset";

				const std::filesystem::recursive_directory_iterator assetIterator(assetDirectory);

				for (const std::filesystem::directory_entry& assetEntry : assetIterator) {
					if (assetEntry.is_directory()) {
						continue;
					}

					const std::string assetPath = assetEntry.path().string();

					const sequence<uint8> data = Filesystem::openBuffer(assetPath);
					const ByteStream<StreamOperation::READ> stream(&data[0], data.count);

					const uint32 signature = stream.peek<uint32>();

					if (signature != AssetSignature::TFO) {
						continue;
					}
				}
			}

			/* const ref<Asset>& getAsset(uint32 assetIdentifier) const {
				return assets[assetIdentifierToAssetIndex[assetIdentifier]];
			}

			const ref<Asset>& operator [](uint32 assetIndex) const {
				return assets[assetIndex];
			} */

		private:
			std::vector<ref<Asset>> assets;
			// std::unordered_map<uint32, uint32> assetIdentifierToAssetIndex;
	};
}