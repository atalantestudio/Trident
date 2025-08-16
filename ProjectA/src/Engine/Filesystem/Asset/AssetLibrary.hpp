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
					const std::string stem = assetEntry.path().stem().string();

					const sequence<uint8> data = Filesystem::openBuffer(assetPath);

					ByteStream<StreamOperation::READ> byteStream(&data[0], data.count);

					const AssetSignature signature = byteStream.peek<AssetSignature>();

					if (signature != AssetSignature::TIM) {
						continue;
					}

					byteStream.jump(sizeof(AssetSignature));

					const ref<ImageAsset> imageAsset = decode(byteStream);

					const uint32 imageAssetIndex = static_cast<uint32>(assets.size());

					assetIdentifierToAssetIndex.insert({imageAsset->identifier, imageAssetIndex});
					imageAssetStemToAssetIndex.insert({stem, imageAssetIndex});

					assets.emplace_back(imageAsset);
				}
			}

			const ref<Asset>& findAsset(uint32 identifier) const {
				const auto iterator = assetIdentifierToAssetIndex.find(identifier);

				ENGINE_ASSERT(iterator != assetIdentifierToAssetIndex.end(), "No asset exists with the provided identifier.");
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				return assets[iterator->second];
			}

			ref<FontAsset> findFontAsset(const std::string& stem) const {
				const auto iterator = fontAssetStemToAssetIndex.find(stem);

				ENGINE_ASSERT(iterator != fontAssetStemToAssetIndex.end(), "No asset exists with the provided identifier.");
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				 return std::static_pointer_cast<FontAsset>(assets[iterator->second]);
			}

			ref<ImageAsset> findImageAsset(const std::string& stem) const {
				const auto iterator = imageAssetStemToAssetIndex.find(stem);

				ENGINE_ASSERT(iterator != imageAssetStemToAssetIndex.end(), "No asset exists with the provided identifier.");
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				 return std::static_pointer_cast<ImageAsset>(assets[iterator->second]);
			}

		private:
			ref<ImageAsset> decode(ByteStream<StreamOperation::READ>& byteStream) const {
				ImageAssetDescriptor imageAssetDescriptor{};
				imageAssetDescriptor.identifier = byteStream.read<uint32>();
				imageAssetDescriptor.version = byteStream.read<VersionNumber>();
				imageAssetDescriptor.format = static_cast<ImageFormat>(byteStream.read<uint32>());
				imageAssetDescriptor.extent = byteStream.read<Extent3D>();
				imageAssetDescriptor.mipLevelCount = byteStream.read<uint32>();
				imageAssetDescriptor.layerCount = byteStream.read<uint32>();

				// TODO: Decode image data.
				// imageAssetDescriptor.data = nullptr;

				BREAKPOINT();

				return createRef<ImageAsset>(imageAssetDescriptor);
			}

		private:
			std::vector<ref<Asset>> assets;
			std::unordered_map<uint32, uint32> assetIdentifierToAssetIndex;
			std::unordered_map<std::string, uint32> fontAssetStemToAssetIndex;
			std::unordered_map<std::string, uint32> imageAssetStemToAssetIndex;
	};
}