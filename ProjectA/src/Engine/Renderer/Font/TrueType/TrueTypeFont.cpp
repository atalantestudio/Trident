#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	TrueTypeFont::TrueTypeFont(std::string_view ttfFilePath) {
		const TTFFile ttfFile = Filesystem::openTemporary<TTFFile>(ttfFilePath);

		sdfGlyphMap = SDFGlyphMapBuildProcess::build(ttfFile);

		ascent = ttfFile.hhea.ascent;
		descent = ttfFile.hhea.descent;
		lineGap = ttfFile.hhea.lineGap;
		unicodeMap = ttfFile.unicodeMap;

		glyphs = sequence<TrueTypeGlyph>(ttfFile.maxp.numGlyphs);

		for (uint16 glyphIndex = 0; glyphIndex < ttfFile.maxp.numGlyphs; glyphIndex += 1) {
			const Glyph& glyph = ttfFile.glyphs[glyphIndex];
			const longHorMetric& hMetric = ttfFile.hMetrics[glyphIndex];
			const SDFGlyph& sdfGlyph = sdfGlyphMap.glyphs[glyphIndex];

			TrueTypeGlyph& trueTypeGlyph = glyphs[glyphIndex];
			trueTypeGlyph.invisible = sdfGlyph.invisible;
			trueTypeGlyph.minCoordinateX = sdfGlyph.minCoordinateX;
			trueTypeGlyph.maxCoordinateX = sdfGlyph.maxCoordinateX;
			trueTypeGlyph.minCoordinateY = sdfGlyph.minCoordinateY;
			trueTypeGlyph.maxCoordinateY = sdfGlyph.maxCoordinateY;
			trueTypeGlyph.width = glyph.max.x - glyph.min.x;
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

	const SDFGlyphMap& TrueTypeFont::getGlyphMap() const {
		return sdfGlyphMap;
	}

	const sequence<TrueTypeGlyph>& TrueTypeFont::getGlyphs() const {
		return glyphs;
	}

	const TrueTypeGlyph& TrueTypeFont::operator [](uint16 codePoint) const {
		const std::list<std::pair<const uint16, uint32>, std::allocator<std::pair<const uint16, uint32>>>::const_iterator glyphIndex = unicodeMap.find(codePoint);

		return glyphs[glyphIndex->second];
	}
}