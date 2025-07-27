#pragma once

#include "ProjectA.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

namespace ProjectA {
	class TTFToTFOProcess : public Process {
		public:
			explicit TTFToTFOProcess() {
				const Stream stream = Filesystem::openStream("../ProjectA/src/Asset/Font/test-regular.ttf");
				const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(stream);

				const SDFGlyphMap sdfGlyphMap = SDFGlyphMapBuildProcess::build(file);

				ImageAssetDescriptor glyphMapImageDescriptor{};
				glyphMapImageDescriptor.format = ImageFormat::R8_UNORM;
				glyphMapImageDescriptor.extent = {sdfGlyphMap.extent.width, sdfGlyphMap.extent.height, 1};
				glyphMapImageDescriptor.mipLevelCount = 1;
				glyphMapImageDescriptor.layerCount = 1;
				glyphMapImageDescriptor.sampleCount = 1;
				glyphMapImageDescriptor.data = sdfGlyphMap.data;

				const ref<ImageAsset> glyphMapImage = createRef<ImageAsset>(glyphMapImageDescriptor);

				FontDescriptor fontDescriptor{};
				fontDescriptor.glyphMapImage = glyphMapImage;
				fontDescriptor.glyphs = sdfGlyphMap.glyphs;
				fontDescriptor.unicodeMap = file.unicodeMap;
				fontDescriptor.lineHeight = file.ascent - file.descent + file.lineGap;

				const ref<Font> font = createRef<Font>(fontDescriptor);

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

				BREAKPOINT();
			}
	};
}