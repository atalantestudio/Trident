#pragma once

#include "ProjectA.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

namespace ProjectA {
	// TODO: Remove.
	class TTFToTFOProcess : public Process {
		public:
			explicit TTFToTFOProcess(std::string_view path, FontAssetDescriptor& fontAssetDescriptor) {
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

				fontAssetDescriptor.glyphMapImage = glyphMapImage;
				fontAssetDescriptor.glyphs = sdfGlyphMap.glyphs;
				fontAssetDescriptor.unicodeMap = file.unicodeMap;
				fontAssetDescriptor.lineHeight = file.ascent - file.descent + file.lineGap;
			}
	};
}