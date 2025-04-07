#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"

namespace ProjectA {
	TrueTypeFont::TrueTypeFont(const std::string& ttfFilePath) {
		const TTFFile ttfFile = Filesystem::load<TTFFile>(ttfFilePath);

		sdfGlyphMap = SDFGlyphMapBuildProcess::build(ttfFile);

		ascent = ttfFile.hhea.ascent;
		descent = ttfFile.hhea.descent;
		lineGap = ttfFile.hhea.lineGap;
		unicodeMap = ttfFile.unicodeMap;

		glyphs.resize(ttfFile.maxp.numGlyphs);

		for (uint16 glyphIndex = 0; glyphIndex < ttfFile.maxp.numGlyphs; glyphIndex += 1) {
			const Glyph& glyph = ttfFile.glyphs[glyphIndex];
			const longHorMetric& hMetric = ttfFile.hMetrics[glyphIndex];
			const SDFGlyph& sdfGlyph = sdfGlyphMap.glyphs[glyphIndex];

			TrueTypeGlyph& trueTypeGlyph = glyphs[glyphIndex];
			trueTypeGlyph.width = static_cast<float32>(sdfGlyph.width - sdfGlyphMap.spreadFactor * 2);// / sdfGlyphMap.glyphMap->getWidth();
			trueTypeGlyph.minCoordinateX = static_cast<float32>(sdfGlyph.xOffset + sdfGlyphMap.spreadFactor) / sdfGlyphMap.glyphMap->getWidth();
			trueTypeGlyph.minCoordinateY = static_cast<float32>(sdfGlyph.lineIndex * sdfGlyphMap.lineHeight) / sdfGlyphMap.glyphMap->getHeight();
			trueTypeGlyph.maxCoordinateX = static_cast<float32>(sdfGlyph.xOffset + sdfGlyph.width - sdfGlyphMap.spreadFactor) / sdfGlyphMap.glyphMap->getWidth();
			trueTypeGlyph.maxCoordinateY = static_cast<float32>((sdfGlyph.lineIndex + 1) * sdfGlyphMap.lineHeight) / sdfGlyphMap.glyphMap->getHeight();
			trueTypeGlyph.advanceWidth = sdfGlyph.advanceWidth;
			trueTypeGlyph.leftSideBearing = sdfGlyph.leftSideBearing;
			trueTypeGlyph.invisible = sdfGlyph.invisible;
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

	const ref<Image>& TrueTypeFont::getSdfImage() const {
		return sdfGlyphMap.glyphMap;
	}

	const chain<TrueTypeGlyph>& TrueTypeFont::getGlyphs() const {
		return glyphs;
	}

	const TrueTypeGlyph& TrueTypeFont::operator [](uint16 codePoint) const {
		const auto glyphIndex = unicodeMap.find(codePoint);

		return glyphs[glyphIndex->second];
	}
}