#pragma once

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/GUI/Font/Font.hpp"
#include "Engine/GUI/Font/TrueType/Glyph.hpp"

namespace ProjectA {
	class TrueTypeFont : public Font {
		public:
			explicit TrueTypeFont(std::string_view path);

			sint16 getAscent() const;

			sint16 getDescent() const;

			sint16 getLineGap() const;

			sint32 getLineHeight() const;

			const SDFGlyphMap& getGlyphMap() const;

			const sequence<Glyph>& getGlyphs() const;

			const Glyph& operator [](uint16 codePoint) const;

		private:
			sint16 ascent;
			sint16 descent;
			sint16 lineGap;
			std::unordered_map<uint16, uint32> unicodeMap;
			sequence<Glyph> glyphs;
			std::unordered_map<uint16, uint16> map;
			SDFGlyphMap sdfGlyphMap;
	};
}