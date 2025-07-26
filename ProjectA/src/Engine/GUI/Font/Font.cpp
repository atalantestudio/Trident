#include "pch.hpp"

#include "Engine/GUI/Font/Font.hpp"

namespace ProjectA {
	Font::Font(const File<FileFormat::TTF>& file) {
		const SDFGlyphMap& sdfGlyphMap = SDFGlyphMapBuildProcess::build(file);

		glyphMap = sequence<Glyph>(file.glyphs.count);
		unicodeMap = file.unicodeMap;
		lineHeight = file.ascent - file.descent + file.lineGap;

		// TODO: Build the glyph list within SDFGlyphMapBuildProcess.
		//       SDF* should not be used here.
		for (uint16 glyphIndex = 0; glyphIndex < file.glyphs.count; glyphIndex += 1) {
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
		}
	}

	const ref<ImageAsset>& Font::getGlyphMapImage() const {
		return glyphMapImage;
	}

	const sequence<Glyph>& Font::getGlyphMap() const {
		return glyphMap;
	}

	uint32 Font::getGlyphCount() const {
		return static_cast<uint32>(glyphMap.count);
	}

	sint32 Font::getLineHeight() const {
		return lineHeight;
	}

	const Glyph& Font::operator [](uint16 codePoint) const {
		const std::list<std::pair<const uint16, uint32>, std::allocator<std::pair<const uint16, uint32>>>::const_iterator glyphIndex = unicodeMap.find(codePoint);

		return glyphMap[glyphIndex->second];
	}
}