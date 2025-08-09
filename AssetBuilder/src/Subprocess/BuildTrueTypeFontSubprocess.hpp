#pragma once

// TODO: Remove this include.
#include "ProjectA.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

namespace ProjectA {
	class BuildTrueTypeFontSubprocess {
		private:
			static constexpr uint32 TFO_SIGNATURE = 0x491b9ec5;

		public:
			explicit BuildTrueTypeFontSubprocess(const std::filesystem::directory_entry& directoryEntry, const std::string& fontDirectoryBuildPath) {
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

				ImageAssetDescriptor glyphMapImageDescriptor{};
				glyphMapImageDescriptor.format = ImageFormat::R8_UNORM;
				glyphMapImageDescriptor.extent = {sdfGlyphMap.extent.width, sdfGlyphMap.extent.height, 1};
				glyphMapImageDescriptor.mipLevelCount = 1;
				glyphMapImageDescriptor.layerCount = 1;
				glyphMapImageDescriptor.sampleCount = 1;
				glyphMapImageDescriptor.data = sdfGlyphMap.data;

				fontDescriptor.glyphMapImage = createRef<ImageAsset>(glyphMapImageDescriptor);

				const uint32 glyphCount = static_cast<uint32>(fontDescriptor.glyphs.count);

				const uint64 outputSize =
					5 * sizeof(uint32) +
					3 * sizeof(sint16) +
					glyphCount * (4 * sizeof(float32) + 3 * sizeof(uint16))
				;

				ByteStream<StreamOperation::WRITE> outputByteStream(outputSize);

				const uint32 assetIdentifier = random<uint32>(min<uint32>(), max<uint32>());

				// TODO: Find glyph map image asset.
				const uint32 glyphMapImageAssetIdentifier = 0;

				outputByteStream.write<uint32>(TFO_SIGNATURE);
				outputByteStream.write<uint32>(assetIdentifier);
				outputByteStream.write<VersionNumber>({0, 0, 0});
				outputByteStream.write<uint32>(glyphMapImageAssetIdentifier);
				outputByteStream.write<sint16>(file.ascent);
				outputByteStream.write<sint16>(file.descent);
				outputByteStream.write<sint16>(file.lineGap);
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

				std::string outputPath = fontDirectoryBuildPath;
				outputPath += directoryEntry.path().stem().string();
				outputPath += ".tfo";

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputByteStream.data, outputByteStream.size);

				BREAKPOINT();
			}
	};
}