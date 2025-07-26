#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	TrueTypeFontClass::TrueTypeFontClass(std::string_view path) {
		const Stream stream = Filesystem::openStream(path);
		const File<FileFormat::TTF> file = Filesystem::decode<FileFormat::TTF>(stream);

		sdfGlyphMap = SDFGlyphMapBuildProcess::build(file);
		unicodeMap = file.unicodeMap;
		glyphs = sequence<Glyph>(file.glyphs.count);
		lineHeight = file.ascent - file.descent + file.lineGap;

		for (uint16 glyphIndex = 0; glyphIndex < file.glyphs.count; glyphIndex += 1) {
			const TrueTypeFont::Glyph& trueTypeGlyph = file.glyphs[glyphIndex];
			const TrueTypeFont::longHorMetric& hMetric = file.hMetrics[glyphIndex];
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

	sint32 TrueTypeFontClass::getLineHeight() const {
		return lineHeight;
	}

	const SDFGlyphMap& TrueTypeFontClass::getGlyphMap() const {
		return sdfGlyphMap;
	}

	const sequence<Glyph>& TrueTypeFontClass::getGlyphs() const {
		return glyphs;
	}

	const Glyph& TrueTypeFontClass::operator [](uint16 codePoint) const {
		const std::list<std::pair<const uint16, uint32>, std::allocator<std::pair<const uint16, uint32>>>::const_iterator glyphIndex = unicodeMap.find(codePoint);

		return glyphs[glyphIndex->second];
	}
}