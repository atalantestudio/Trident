#pragma once

#include "ProjectA.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

namespace ProjectA {
	class TTFToTFOProcess : public Process {
		public:
			explicit TTFToTFOProcess(std::string_view path, FontDescriptor& fontDescriptor) {
				const Stream stream = Filesystem::openStream(path);
				const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(stream);

				SDFGlyphMap sdfGlyphMap;

				SDFGlyphMapBuildProcess(file, sdfGlyphMap);

				ImageAssetDescriptor glyphMapImageDescriptor{};
				glyphMapImageDescriptor.format = ImageFormat::R8_UNORM;
				glyphMapImageDescriptor.extent = {sdfGlyphMap.extent.width, sdfGlyphMap.extent.height, 1};
				glyphMapImageDescriptor.mipLevelCount = 1;
				glyphMapImageDescriptor.layerCount = 1;
				glyphMapImageDescriptor.sampleCount = 1;
				glyphMapImageDescriptor.data = sdfGlyphMap.data;

				const ref<ImageAsset> glyphMapImage = createRef<ImageAsset>(glyphMapImageDescriptor);

				fontDescriptor.glyphMapImage = glyphMapImage;
				fontDescriptor.glyphs = sdfGlyphMap.glyphs;
				fontDescriptor.unicodeMap = file.unicodeMap;
				fontDescriptor.lineHeight = file.ascent - file.descent + file.lineGap;

				// TODO: Remove.
				/* for (uint16 glyphIndex = 0; glyphIndex < file.glyphs.count; glyphIndex += 1) {
					const TrueTypeFont::Glyph& trueTypeGlyph = file.glyphs[glyphIndex];
					const TrueTypeFont::longHorMetric& hMetric = file.hMetrics[glyphIndex];
					const SDFGlyph& sdfGlyph = sdfGlyphMap.glyphs[glyphIndex];

					Glyph& glyph = glyphMap[glyphIndex];
					glyph.whitespace = sdfGlyph.whitespace;
					glyph.minCoordinate = sdfGlyph.minCoordinate;
					glyph.maxCoordinate = sdfGlyph.maxCoordinate;
					glyph.width = trueTypeGlyph.max.x - trueTypeGlyph.min.x;
					glyph.advanceWidth = hMetric.advanceWidth;
					glyph.leftSideBearing = hMetric.leftSideBearing;
				} */
			}
	};
}