#pragma once

#include "ProjectA.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

namespace ProjectA {
	class TTFToTFOProcess : public Process {
		public:
			explicit TTFToTFOProcess(std::string_view path, FontDescriptor& fontDescriptor) {
				const sequence<uint8> data = Filesystem::openBuffer(path);
				const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(data);

				SDFGlyphMap sdfGlyphMap;

				SDFGlyphMapBuildProcess(file, sdfGlyphMap);

				ImageAssetDescriptor glyphMapImageDescriptor{};
				glyphMapImageDescriptor.format = ImageFormat::R8_UNORM;
				glyphMapImageDescriptor.extent = {sdfGlyphMap.extent.width, sdfGlyphMap.extent.height, 1};
				glyphMapImageDescriptor.mipLevelCount = 1;
				glyphMapImageDescriptor.layerCount = 1;
				glyphMapImageDescriptor.data = sdfGlyphMap.data;

				const ref<ImageAsset> glyphMapImage = createRef<ImageAsset>(glyphMapImageDescriptor);

				fontDescriptor.glyphMapImage = glyphMapImage;
				fontDescriptor.glyphs = sdfGlyphMap.glyphs;
				fontDescriptor.unicodeMap = file.unicodeMap;
				fontDescriptor.lineHeight = file.ascent - file.descent + file.lineGap;
			}
	};
}