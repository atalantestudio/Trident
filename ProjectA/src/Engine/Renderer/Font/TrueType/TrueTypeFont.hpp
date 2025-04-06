#pragma once

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/Renderer/Font/Font.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeGlyph.hpp"

namespace ProjectA {
	class TrueTypeFont : public Font {
		public:
			explicit TrueTypeFont(const std::string& ttfFilePath);

			sint16 getAscent() const;

			sint16 getDescent() const;

			sint16 getLineGap() const;

			sint32 getLineHeight() const;

			const ref<Image>& getSdfImage() const;

			const chain<TrueTypeGlyph>& getGlyphs() const;

			const TrueTypeGlyph& operator [](uint16 codePoint) const;

		private:
			sint16 ascent;
			sint16 descent;
			sint16 lineGap;
			std::unordered_map<uint16, uint32> unicodeMap;
			chain<TrueTypeGlyph> glyphs;
			std::unordered_map<uint16, uint16> map;
			SDFGlyphMap sdfGlyphMap;
	};
}