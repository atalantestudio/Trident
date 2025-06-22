#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	TrueTypeFont::TrueTypeFont(std::string_view ttfFilePath) {
		const TTFFile ttfFile = Filesystem::openTemporary<TTFFile>(ttfFilePath);

		sdfGlyphMap = SDFGlyphMapBuildProcess::build(ttfFile);

		ascent = ttfFile.ascent;
		descent = ttfFile.descent;
		lineGap = ttfFile.lineGap;
		unicodeMap = ttfFile.unicodeMap;

		glyphs = sequence<Glyph>(ttfFile.glyphs.count);

		for (uint16 glyphIndex = 0; glyphIndex < ttfFile.glyphs.count; glyphIndex += 1) {
			const TrueTypeGlyph& trueTypeGlyph = ttfFile.glyphs[glyphIndex];
			const longHorMetric& hMetric = ttfFile.hMetrics[glyphIndex];
			const SDFGlyph& sdfGlyph = sdfGlyphMap.glyphs[glyphIndex];

			Glyph& glyph = glyphs[glyphIndex];
			glyph.invisible = sdfGlyph.invisible;
			glyph.minCoordinate = sdfGlyph.minCoordinate;
			glyph.maxCoordinate = sdfGlyph.maxCoordinate;
			glyph.width = trueTypeGlyph.max.x - trueTypeGlyph.min.x;
			glyph.advanceWidth = hMetric.advanceWidth;
			glyph.leftSideBearing = hMetric.leftSideBearing;
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

	const SDFGlyphMap& TrueTypeFont::getGlyphMap() const {
		return sdfGlyphMap;
	}

	const sequence<Glyph>& TrueTypeFont::getGlyphs() const {
		return glyphs;
	}

	const Glyph& TrueTypeFont::operator [](uint16 codePoint) const {
		const std::list<std::pair<const uint16, uint32>, std::allocator<std::pair<const uint16, uint32>>>::const_iterator glyphIndex = unicodeMap.find(codePoint);

		return glyphs[glyphIndex->second];
	}
}