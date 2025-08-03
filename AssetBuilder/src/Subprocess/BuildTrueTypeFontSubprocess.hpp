#pragma once

// TODO: Remove this include.
#include "ProjectA.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

namespace ProjectA {
	class BuildTrueTypeFontSubprocess {
		public:
			explicit BuildTrueTypeFontSubprocess(const std::string& path) {
				const Stream stream = Filesystem::openStream(path);

				if (!Filesystem::validateSignature<FileFormat::TTF>(stream)) {
					ENGINE_INFO("Ignored non-TTF file \"[]\".", path);

					return;
				}

				const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(stream);

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

				// BREAKPOINT();
			}
	};
}