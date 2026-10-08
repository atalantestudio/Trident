// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Font/BuildFont.hpp"

namespace Trident {
	class FontProcess : public Process {
		public:
			static ByteStream<StreamOperation::WRITE> encodeFontAsset(uint32 identifier, const BuildFont& font) {
				const uint64 streamSize = calculateStreamSize(font);

				ByteStream<StreamOperation::WRITE> stream(streamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::FONT);
				stream.write<uint32>(identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<uint32>(font.glyphMapImage->identifier);
				stream.write<sint16>(font.ascent);
				stream.write<sint16>(font.descent);
				stream.write<sint16>(font.lineGap);
				stream.write<uint16>(font.unitsPerEm);
				stream.write<uint32>(static_cast<uint32>(font.glyphs.count()));
				stream.write<uint32>(static_cast<uint32>(font.unicodeMap.size()));
				stream.write<uint32>(static_cast<uint32>(font.kerningPairs.size()));

				for (const Glyph& glyph : font.glyphs) {
					stream.write<vec2<float32>>(glyph.minCoordinate);
					stream.write<vec2<float32>>(glyph.maxCoordinate);
					stream.write<uint16>(glyph.width);
					stream.write<uint16>(glyph.height);
					stream.write<sint16>(glyph.yMax);
					stream.write<uint16>(glyph.advanceWidth);
					stream.write<sint16>(glyph.leftSideBearing);
				}

				for (const std::pair<uint16, uint32> unicodeEntry : font.unicodeMap) {
					stream.write<uint16>(unicodeEntry.first);
					stream.write<uint32>(unicodeEntry.second);
				}

				for (const std::pair<GlyphPair, sint16> kerningPair : font.kerningPairs) {
					stream.write<GlyphPair>(kerningPair.first);
					stream.write<sint16>(kerningPair.second);
				}

				return stream;
			}

			static void createFontAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, std::filesystem::path assetOutputPath, AssetBuilderLibrary* assetLibrary) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tfo", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.getByteOffset());

				assetLibrary->registerAsset(identifier, assetName);
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Font/";
				const view<char8> assetName = getAssetName();

				std::filesystem::path outputPath = assetOutputPath / std::string(assetName);
				outputPath.replace_extension("tfo");

				if (std::filesystem::exists(outputPath)) {
					return 0;
				}

				const uint32 identifier = AssetLibrary::createAssetIdentifier();
				const BuildFont font = createFont(assetLibrary);
				const ByteStream<StreamOperation::WRITE> stream = encodeFontAsset(identifier, font);

				createFontAsset(identifier, stream, assetName, assetOutputPath, assetLibrary);

				ENGINE_INFO("Generated font asset \"[]\" (ID: []).", assetName, identifier);

				return 0;
			}

		private:
			static uint64 calculateStreamSize(const BuildFont& font) {
				uint64 streamSize = (
					sizeof(AssetHeader) +
					sizeof(uint32) * 4 +
					sizeof(sint16) * 3 +
					sizeof(uint16) +
					font.glyphs.count() * (sizeof(vec2<float32>) * 2 + sizeof(uint16) * 3 + sizeof(sint16) * 2) +
					font.unicodeMap.size() * (sizeof(uint16) + sizeof(uint32)) +
					font.kerningPairs.size() * (sizeof(GlyphPair) + sizeof(sint16))
				);

				return streamSize;
			}

		public:
			explicit FontProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

		protected:
			virtual view<char8> getAssetName() const = 0;

			virtual BuildFont createFont(AssetBuilderLibrary* assetLibrary) const = 0;

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}