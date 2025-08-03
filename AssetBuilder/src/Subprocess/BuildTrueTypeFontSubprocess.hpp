#pragma once

// TODO: Remove this include.
#include "ProjectA.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

namespace ProjectA {
	class BuildTrueTypeFontSubprocess {
		public:
			explicit BuildTrueTypeFontSubprocess(const std::filesystem::directory_entry& directoryEntry, const std::string& fontDirectoryBuildPath) {
				const std::string path = directoryEntry.path().string();

				const Stream inputStream = Filesystem::openStream(path);

				if (!Filesystem::validateSignature<FileFormat::TTF>(inputStream)) {
					ENGINE_INFO("Ignored non-TTF file \"[]\".", path);

					return;
				}

				const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(inputStream);

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

				const Asset asset;

				sequence<uint8> streamBuffer(4 * sizeof(uint32));

				Stream outputStream(streamBuffer);
				ByteStream byteStream = outputStream.asByteStream();

				// byteStream.write<uint32>(getSignature<AssetType::FONT>());
				// byteStream.write<uint32>(asset.identifier);
				// byteStream.write<uint32>(asset.version);
				// byteStream.write<uint32>(fontDescriptor.glyphs.count);

				std::string outputPath = fontDirectoryBuildPath;
				outputPath += directoryEntry.path().stem().string();
				outputPath += ".tfo";

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(outputStream.getBuffer());

				BREAKPOINT();
			}
	};
}