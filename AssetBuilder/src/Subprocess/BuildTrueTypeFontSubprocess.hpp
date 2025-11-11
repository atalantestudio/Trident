#pragma once

#include "BuildGlyphMapImageSubprocess.hpp"
#include "../Process/GenerateSDFGlyphMapProcess.hpp"

namespace ProjectA {
	class BuildTrueTypeFontSubprocess {
		public:
			explicit BuildTrueTypeFontSubprocess(const std::filesystem::directory_entry& directoryEntry, const std::string& assetOutputPath) {
				const str8 path = directoryEntry.path().string();
				const sequence<uint8> inputData = Filesystem::openBuffer(path);
				const ByteStream<StreamOperation::READ> inputByteStream(&inputData[0], inputData.count(), Endianness::BIG_ENDIAN);

				if (!Filesystem::validateSignature<FileFormat::TTF>(inputByteStream)) {
					ENGINE_INFO("Ignoring non-TTF file \"[]\".", path);

					return;
				}

				const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(inputData);

				FontAssetDescriptor fontAssetDescriptor{};
				fontAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
				fontAssetDescriptor.version = {1, 0, 0};
				fontAssetDescriptor.lineHeight = file.ascent - file.descent + file.lineGap;
				fontAssetDescriptor.unicodeMap = file.unicodeMap;

				SDFGlyphMap sdfGlyphMap;

				GenerateSDFGlyphMapProcess(file, sdfGlyphMap);

				fontAssetDescriptor.glyphs = sdfGlyphMap.glyphs;

				ImageAssetDescriptor glyphMapImageAssetDescriptor{};
				glyphMapImageAssetDescriptor.format = ImageFormat::R8_UNORM;
				glyphMapImageAssetDescriptor.extent = {sdfGlyphMap.extent.width, sdfGlyphMap.extent.height, 1};
				glyphMapImageAssetDescriptor.mipLevelCount = 1;
				glyphMapImageAssetDescriptor.layerCount = 1;
				glyphMapImageAssetDescriptor.data = sdfGlyphMap.data;

				fontAssetDescriptor.glyphMapImage = createRef<ImageAsset>(glyphMapImageAssetDescriptor);

				const uint32 glyphCount = static_cast<uint32>(fontAssetDescriptor.glyphs.count());
				const uint64 outputSize =
					7 * sizeof(uint32) + (4 * sizeof(float32) + 3 * sizeof(uint16)) * glyphCount +
					(sizeof(uint16) + sizeof(uint32)) * file.unicodeMap.size();

				uint32 glyphMapImageAssetIdentifier;

				const BuildGlyphMapImageSubprocess buildGlyphMapImageSubprocess(assetOutputPath, directoryEntry.path().stem().string(), fontAssetDescriptor.glyphMapImage, glyphMapImageAssetIdentifier);

				ByteStream<StreamOperation::WRITE> outputByteStream(outputSize, Endianness::LITTLE_ENDIAN);
				outputByteStream.write<AssetSignature>(AssetSignature::TFO);
				outputByteStream.write<uint32>(fontAssetDescriptor.identifier);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(glyphMapImageAssetIdentifier);
				outputByteStream.write<sint16>(file.ascent);
				outputByteStream.write<sint16>(file.descent);
				outputByteStream.write<sint16>(file.lineGap);
				outputByteStream.jump(sizeof(uint16));
				outputByteStream.write<uint32>(glyphCount);

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<float32>(glyph.minCoordinate.x);
					outputByteStream.write<float32>(glyph.minCoordinate.y);
					outputByteStream.write<float32>(glyph.maxCoordinate.x);
					outputByteStream.write<float32>(glyph.maxCoordinate.y);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<uint16>(glyph.width);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<uint16>(glyph.advanceWidth);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontAssetDescriptor.glyphs[glyphIndex];

					outputByteStream.write<sint16>(glyph.leftSideBearing);
				}

				for (const auto& unicodeEntry : file.unicodeMap) {
					outputByteStream.write<uint16>(unicodeEntry.first);
				}

				for (const auto& unicodeEntry : file.unicodeMap) {
					outputByteStream.write<uint32>(unicodeEntry.second);
				}

				const std::string outputPath = assetOutputPath + "Font/" + directoryEntry.path().stem().string() + ".tfo";

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputByteStream.size);

				ENGINE_INFO("Generated font asset (ID: []).", fontAssetDescriptor.identifier);
			}
	};
}