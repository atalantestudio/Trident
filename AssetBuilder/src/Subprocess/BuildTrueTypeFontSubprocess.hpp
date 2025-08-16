#pragma once

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

#include "BuildGlyphMapImageSubprocess.hpp"

namespace ProjectA {
	class BuildTrueTypeFontSubprocess {
		public:
			explicit BuildTrueTypeFontSubprocess(const std::filesystem::directory_entry& directoryEntry, const std::string& assetOutputPath) {
				const std::string path = directoryEntry.path().string();
				const sequence<uint8> inputData = Filesystem::openBuffer(path);
				const ByteStream<StreamOperation::READ> inputByteStream(&inputData[0], inputData.count);

				if (!Filesystem::validateSignature<FileFormat::TTF>(inputByteStream)) {
					ENGINE_INFO("Ignored non-TTF file \"[]\".", path);

					return;
				}

				const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(inputData);

				FontDescriptor fontDescriptor{};
				fontDescriptor.unicodeMap = file.unicodeMap;
				fontDescriptor.lineHeight = file.ascent - file.descent + file.lineGap;

				SDFGlyphMap sdfGlyphMap;

				SDFGlyphMapBuildProcess(file, sdfGlyphMap);

				fontDescriptor.glyphs = sdfGlyphMap.glyphs;

				ImageAssetDescriptor glyphMapImageAssetDescriptor{};
				glyphMapImageAssetDescriptor.format = ImageFormat::R8_UNORM;
				glyphMapImageAssetDescriptor.extent = {sdfGlyphMap.extent.width, sdfGlyphMap.extent.height, 1};
				glyphMapImageAssetDescriptor.mipLevelCount = 1;
				glyphMapImageAssetDescriptor.layerCount = 1;
				glyphMapImageAssetDescriptor.sampleCount = 1;
				glyphMapImageAssetDescriptor.data = sdfGlyphMap.data;

				fontDescriptor.glyphMapImage = createRef<ImageAsset>(glyphMapImageAssetDescriptor);

				const uint32 glyphCount = static_cast<uint32>(fontDescriptor.glyphs.count);

				const uint64 outputSize = 7 * sizeof(uint32) + glyphCount * (4 * sizeof(float32) + 3 * sizeof(uint16));

				ByteStream<StreamOperation::WRITE> outputByteStream(outputSize);

				const uint32 assetIdentifier = AssetLibrary::createAssetIdentifier();

				uint32 glyphMapImageAssetIdentifier;

				const BuildGlyphMapImageSubprocess buildGlyphMapImageSubprocess(assetOutputPath, directoryEntry.path().stem().string(), fontDescriptor.glyphMapImage, glyphMapImageAssetIdentifier);

				outputByteStream.write<uint32>(AssetSignature::TFO);
				outputByteStream.write<uint32>(assetIdentifier);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(glyphMapImageAssetIdentifier);
				outputByteStream.write<sint16>(file.ascent);
				outputByteStream.write<sint16>(file.descent);
				outputByteStream.write<sint16>(file.lineGap);
				outputByteStream.jump(sizeof(sint16));
				outputByteStream.write<uint32>(glyphCount);

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontDescriptor.glyphs[glyphIndex];

					outputByteStream.write<float32>(glyph.minCoordinate.x);
					outputByteStream.write<float32>(glyph.minCoordinate.y);
					outputByteStream.write<float32>(glyph.maxCoordinate.x);
					outputByteStream.write<float32>(glyph.maxCoordinate.y);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontDescriptor.glyphs[glyphIndex];

					outputByteStream.write<uint16>(glyph.width);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontDescriptor.glyphs[glyphIndex];

					outputByteStream.write<uint16>(glyph.advanceWidth);
				}

				for (uint32 glyphIndex = 0; glyphIndex < glyphCount; glyphIndex += 1) {
					const Glyph& glyph = fontDescriptor.glyphs[glyphIndex];

					outputByteStream.write<uint16>(glyph.leftSideBearing);
				}

				const std::string outputPath = assetOutputPath + "Font/" + directoryEntry.path().stem().string() + ".tfo";

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputByteStream.size);
			}
	};
}