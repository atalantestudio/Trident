#pragma once

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/GUI/Font/Font.hpp"
#include "Engine/GUI/Font/TrueType/Glyph.hpp"

namespace ProjectA {
	class TrueTypeFontClass : public Font {
		public:
			explicit TrueTypeFontClass(std::string_view path);

			sint32 getLineHeight() const;

			const SDFGlyphMap& getGlyphMap() const;

			const sequence<Glyph>& getGlyphs() const;

			const Glyph& operator [](uint16 codePoint) const;

		private:
			uint32 lineHeight;
			std::unordered_map<uint16, uint32> unicodeMap;
			sequence<Glyph> glyphs;
			std::unordered_map<uint16, uint16> map;
			SDFGlyphMap sdfGlyphMap;
	};
}