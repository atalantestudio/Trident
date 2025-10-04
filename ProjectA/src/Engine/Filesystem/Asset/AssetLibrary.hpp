#pragma once

#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/AssetSignature.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	class AssetLibrary {
		public:
			// https://en.wikipedia.org/wiki/Ostrich_algorithm
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
				std::filesystem::recursive_directory_iterator assetIterator;

				assetIterator = std::filesystem::recursive_directory_iterator(std::filesystem::current_path() / "Asset/Image");

				for (const std::filesystem::directory_entry& assetEntry : assetIterator) {
					loadImageAsset(assetEntry);
				}

				assetIterator = std::filesystem::recursive_directory_iterator(std::filesystem::current_path() / "Asset/Font");

				for (const std::filesystem::directory_entry& assetEntry : assetIterator) {
					loadFontAsset(assetEntry);
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

				ENGINE_ASSERT(iterator != fontAssetStemToAssetIndex.end(), "Cannot locate font asset \"[]\".", stem);
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				return std::static_pointer_cast<FontAsset>(assets[iterator->second]);
			}

			ref<ImageAsset> findImageAsset(const std::string& stem) const {
				const auto iterator = imageAssetStemToAssetIndex.find(stem);

				ENGINE_ASSERT(iterator != imageAssetStemToAssetIndex.end(), "Cannot locate image asset \"[]\".", stem);
				ENGINE_ASSERT(iterator->second < assets.size(), "The found asset index is overflowing.");

				return std::static_pointer_cast<ImageAsset>(assets[iterator->second]);
			}

		private:
			void loadFontAsset(const std::filesystem::directory_entry& assetEntry) {
				if (assetEntry.is_directory()) {
					return;
				}

				const std::string assetPath = assetEntry.path().string();

				const sequence<uint8> data = Filesystem::openBuffer(assetPath);

				ByteStream<StreamOperation::READ> byteStream(&data[0], data.count, Endianness::LITTLE_ENDIAN);

				const AssetSignature signature = byteStream.peek<AssetSignature>();

				if (signature != AssetSignature::TFO) {
					return;
				}

				byteStream.jump(sizeof(AssetSignature));

				const ref<FontAsset> fontAsset = decodeFontAsset(byteStream);

				const uint32 fontAssetIndex = static_cast<uint32>(assets.size());

				const std::string stem = assetEntry.path().stem().string();

				assetIdentifierToAssetIndex.insert({fontAsset->identifier, fontAssetIndex});
				fontAssetStemToAssetIndex.insert({stem, fontAssetIndex});

				assets.emplace_back(fontAsset);
			}

			void loadImageAsset(const std::filesystem::directory_entry& assetEntry) {
				if (assetEntry.is_directory()) {
					return;
				}

				const std::string assetPath = assetEntry.path().string();

				const sequence<uint8> data = Filesystem::openBuffer(assetPath);

				ByteStream<StreamOperation::READ> byteStream(&data[0], data.count, Endianness::LITTLE_ENDIAN);

				const AssetSignature signature = byteStream.peek<AssetSignature>();

				if (signature != AssetSignature::TIM) {
					return;
				}

				byteStream.jump(sizeof(AssetSignature));

				const ref<ImageAsset> imageAsset = decodeImageAsset(byteStream);

				const uint32 imageAssetIndex = static_cast<uint32>(assets.size());

				const std::string stem = assetEntry.path().stem().string();

				assetIdentifierToAssetIndex.insert({imageAsset->identifier, imageAssetIndex});
				imageAssetStemToAssetIndex.insert({stem, imageAssetIndex});

				assets.emplace_back(imageAsset);
			}

			ref<FontAsset> decodeFontAsset(ByteStream<StreamOperation::READ>& byteStream) const {
				FontAssetDescriptor fontAssetDescriptor{};
				fontAssetDescriptor.identifier = byteStream.read<uint32>();
				fontAssetDescriptor.version = byteStream.read<VersionNumber>();

				const uint32 glyphMapImageAssetIdentifier = byteStream.read<uint32>();

				fontAssetDescriptor.glyphMapImage = std::static_pointer_cast<ImageAsset>(findAsset(glyphMapImageAssetIdentifier));

				const sint16 ascent = byteStream.read<sint16>();
				const sint16 descent = byteStream.read<sint16>();
				const sint16 lineGap = byteStream.read<sint16>();

				fontAssetDescriptor.lineHeight = ascent - descent + lineGap;

				byteStream.jump(sizeof(uint16));

				const uint32 glyphCount = byteStream.read<uint32>();

				fontAssetDescriptor.glyphs = sequence<Glyph>(glyphCount);

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.minCoordinate.x = byteStream.read<float32>();
					glyph.minCoordinate.y = byteStream.read<float32>();
					glyph.maxCoordinate.x = byteStream.read<float32>();
					glyph.maxCoordinate.y = byteStream.read<float32>();
					glyph.whitespace = (glyph.minCoordinate.x == glyph.maxCoordinate.x && glyph.minCoordinate.y == glyph.maxCoordinate.y);
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.width = byteStream.read<uint16>();
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.advanceWidth = byteStream.read<uint16>();
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.leftSideBearing = byteStream.read<sint16>();
				}

				const uint64 unicodeMapOffset = byteStream.getByteOffset();
				const uint64 unicodeMapSize = byteStream.getRemainingByteSize() / (sizeof(uint16) + sizeof(uint32));

				fontAssetDescriptor.unicodeMap.reserve(unicodeMapSize);

				for (uint64 unicodeEntryIndex = 0; unicodeEntryIndex < unicodeMapSize; unicodeEntryIndex += 1) {
					byteStream.seek(unicodeMapOffset + unicodeMapSize * sizeof(uint16) + unicodeEntryIndex * sizeof(uint32));

					const uint32 glyphIndex = byteStream.peek<uint32>();

					byteStream.seek(unicodeMapOffset + unicodeEntryIndex * sizeof(uint16));

					const uint16 codePoint = byteStream.read<uint16>();

					fontAssetDescriptor.unicodeMap.insert({codePoint, glyphIndex});
				}

				return createRef<FontAsset>(fontAssetDescriptor);
			}

			ref<ImageAsset> decodeImageAsset(ByteStream<StreamOperation::READ>& byteStream) const {
				ImageAssetDescriptor imageAssetDescriptor{};
				imageAssetDescriptor.identifier = byteStream.read<uint32>();
				imageAssetDescriptor.version = byteStream.read<VersionNumber>();
				imageAssetDescriptor.format = static_cast<ImageFormat>(byteStream.read<uint32>());

				ENGINE_ASSERT(imageAssetDescriptor.format == ImageFormat::R8G8B8A8_UNORM, "Image formats other than R8G8B8A8_UNORM are not supported yet.");

				imageAssetDescriptor.extent = byteStream.read<Extent3D>();
				imageAssetDescriptor.mipLevelCount = byteStream.read<uint32>();
				imageAssetDescriptor.layerCount = byteStream.read<uint32>();

				static constexpr uint8 CHANNEL_COUNT = 4;

				imageAssetDescriptor.data = sequence<uint8>(imageAssetDescriptor.layerCount * imageAssetDescriptor.extent.width * imageAssetDescriptor.extent.height * imageAssetDescriptor.extent.depth * CHANNEL_COUNT);

				const uint64 maxIterationCount = byteStream.getRemainingByteSize();

				Filesystem::QOIDecoderContentOnly(byteStream, imageAssetDescriptor.data, CHANNEL_COUNT, maxIterationCount);

				return createRef<ImageAsset>(imageAssetDescriptor);
			}

		private:
			std::vector<ref<Asset>> assets;
			std::unordered_map<uint32, uint32> assetIdentifierToAssetIndex;
			std::unordered_map<std::string, uint32> fontAssetStemToAssetIndex;
			std::unordered_map<std::string, uint32> imageAssetStemToAssetIndex;
	};
}