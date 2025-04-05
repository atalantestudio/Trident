#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	TrueTypeFont::TrueTypeFont(const std::string& ttfFilePath) {
		const TTFFile ttfFile = Filesystem::load<TTFFile>(ttfFilePath);

		ascent = ttfFile.hhea.ascent;
		descent = ttfFile.hhea.descent;
		lineGap = ttfFile.hhea.lineGap;
		unicodeMap = ttfFile.unicodeMap;

		glyphs.resize(ttfFile.maxp.numGlyphs);

		for (uint16 glyphIndex = 0; glyphIndex < ttfFile.maxp.numGlyphs; glyphIndex += 1) {
			const Glyph& glyph = ttfFile.glyphs[glyphIndex];
			const longHorMetric& hMetric = ttfFile.hMetrics[glyphIndex];

			TrueTypeGlyph& trueTypeGlyph = glyphs[glyphIndex];
			trueTypeGlyph.imageCoordinateX = 0;
			trueTypeGlyph.imageCoordinateY = 0;
			trueTypeGlyph.advanceWidth = hMetric.advanceWidth;
			trueTypeGlyph.leftSideBearing = hMetric.leftSideBearing;
		}
	}

	sint16 TrueTypeFont::getAscent() const {
		return ascent;
	}

	sint16 TrueTypeFont::getDescent() const {
		return descent;
	}

	sint16 TrueTypeFont::getLineGap() const {
		return lineGap;
	}

	sint32 TrueTypeFont::getLineHeight() const {
		return ascent - descent + lineGap;
	}

	const chain<TrueTypeGlyph>& TrueTypeFont::getGlyphs() const {
		return glyphs;
	}

	const TrueTypeGlyph& TrueTypeFont::operator [](uint16 codePoint) {
		const uint32 glyphIndex = unicodeMap[codePoint];

		return glyphs[glyphIndex];
	}
}