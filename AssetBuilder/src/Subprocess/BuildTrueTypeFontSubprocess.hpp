// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Process/BuildImageAssetProcess.hpp"
#include "Process/GenerateSDFGlyphMapProcess.hpp"
#include "../File/TTF.hpp"

namespace Trident {
	class BuildTrueTypeFontSubprocess {
		public:
			explicit BuildTrueTypeFontSubprocess(const std::filesystem::directory_entry& directoryEntry, const std::filesystem::path& assetOutputPath) {
				const sequence<char8> path = directoryEntry.path().string();
				const std::string assetNameString = directoryEntry.path().stem().string();
				const view<char8> assetName(&assetNameString[0], assetNameString.size());
				const sequence<uint8> inputData = Filesystem::openByteBuffer(path);
				const ByteStream<StreamOperation::READ> inputByteStream(&inputData[0], inputData.count(), Endianness::BIG_ENDIAN);

				if (!Decoder<FileFormat::TTF>::validateSignature(inputByteStream)) {
					ENGINE_INFO("Ignoring non-TTF file \"[]\".", path);

					return;
				}

				const File<FileFormat::TTF> file = Decoder<FileFormat::TTF>::decode(inputData);

				SDFGlyphMap sdfGlyphMap;

				GenerateSDFGlyphMapProcess(file, sdfGlyphMap);

				ImageAssetDescriptor imageAssetDescriptor{};
				imageAssetDescriptor.format = ImageFormat::R8_UNORM;
				imageAssetDescriptor.extent = {sdfGlyphMap.extent.width, sdfGlyphMap.extent.height, 1};
				imageAssetDescriptor.mipLevelCount = 1;
				imageAssetDescriptor.layerCount = 1;
				imageAssetDescriptor.data = sdfGlyphMap.data;

				BuildImageAssetProcess::encodeImageAsset(assetOutputPath, assetName, imageAssetDescriptor);

				FontAssetDescriptor fontAssetDescriptor{};
				fontAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
				fontAssetDescriptor.version = {1, 0, 0};
				fontAssetDescriptor.glyphMapImage = nullptr;
				fontAssetDescriptor.lineHeight = file.ascent - file.descent + file.lineGap;
				fontAssetDescriptor.glyphs = sdfGlyphMap.glyphs;
				fontAssetDescriptor.unicodeMap = file.unicodeMap;

				const uint32 glyphCount = static_cast<uint32>(fontAssetDescriptor.glyphs.count());
				const uint64 outputSize =
					sizeof(uint32) * 7 +
					(sizeof(svec2<float32>) * 2 + sizeof(uint16) * 3 + sizeof(sint16) * 2) * glyphCount +
					(sizeof(uint16) + sizeof(uint32)) * file.unicodeMap.size();

				ByteStream<StreamOperation::WRITE> outputByteStream(outputSize, Endianness::LITTLE_ENDIAN);
				outputByteStream.write<AssetSignature>(AssetSignature::FONT_ASSET);
				outputByteStream.write<uint32>(fontAssetDescriptor.identifier);
				outputByteStream.write<VersionNumber>({1, 0, 0});
				outputByteStream.write<uint32>(imageAssetDescriptor.identifier);
				outputByteStream.write<sint16>(file.ascent);
				outputByteStream.write<sint16>(file.descent);
				outputByteStream.write<sint16>(file.lineGap);
				outputByteStream.jump(sizeof(uint16));
				outputByteStream.write<uint32>(glyphCount);

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<float32>(glyph.minCoordinate.x);
					outputByteStream.write<float32>(glyph.minCoordinate.y);
					outputByteStream.write<float32>(glyph.maxCoordinate.x);
					outputByteStream.write<float32>(glyph.maxCoordinate.y);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<uint16>(glyph.width);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<uint16>(glyph.advanceWidth);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<uint16>(glyph.height);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<sint16>(glyph.leftSideBearing);
				}

				for (const Glyph& glyph : fontAssetDescriptor.glyphs) {
					outputByteStream.write<sint16>(glyph.topSideBearing);
				}

				for (const std::pair<uint16, uint32>& unicodeEntry : file.unicodeMap) {
					outputByteStream.write<uint16>(unicodeEntry.first);
				}

				for (const std::pair<uint16, uint32>& unicodeEntry : file.unicodeMap) {
					outputByteStream.write<uint32>(unicodeEntry.second);
				}

				const std::string stem = directoryEntry.path().stem().string();
				const std::string assetOutputPathString = assetOutputPath.string();

				// Count '\0'.
				sequence<char8> outputPath(assetOutputPathString.size() + stem.size() + 10);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy_n("Font/", 5, &outputPath[assetOutputPathString.size()]);
				std::copy(stem.begin(), stem.end(), &outputPath[assetOutputPathString.size() + 5]);
				std::copy_n(".tfo", 4, &outputPath[assetOutputPathString.size() + stem.size() + 5]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputByteStream.size);

				ENGINE_INFO("Generated True-Type font asset \"[]\" (ID: []).", outputPath, fontAssetDescriptor.identifier);
			}
	};
}