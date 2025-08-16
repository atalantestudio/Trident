#pragma once

#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
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

					if (signature != AssetSignature::TIM) {
						continue;
					}
				}
			}

			const ref<Asset>& findAsset(uint32 identifier) const {
				const auto iterator = assetIdentifierToAssetIndex.find(identifier);

				ENGINE_ASSERT(iterator != assetIdentifierToAssetIndex.end(), "No asset exists with the provided identifier.");
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				return assets[iterator->second];
			}

			ref<FontAsset> findFontAsset(std::string_view stem) const {
				const auto iterator = fontAssetStemToAssetIndex.find(stem);

				ENGINE_ASSERT(iterator != fontAssetStemToAssetIndex.end(), "No asset exists with the provided identifier.");
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				 return std::static_pointer_cast<FontAsset>(assets[iterator->second]);
			}

			ref<ImageAsset> findImageAsset(std::string_view stem) const {
				const auto iterator = imageAssetStemToAssetIndex.find(stem);

				ENGINE_ASSERT(iterator != imageAssetStemToAssetIndex.end(), "No asset exists with the provided identifier.");
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				 return std::static_pointer_cast<ImageAsset>(assets[iterator->second]);
			}

		private:
			std::vector<ref<Asset>> assets;
			std::unordered_map<uint32, uint32> assetIdentifierToAssetIndex;
			std::unordered_map<std::string_view, uint32> fontAssetStemToAssetIndex;
			std::unordered_map<std::string_view, uint32> imageAssetStemToAssetIndex;
	};
}