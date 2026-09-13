// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Font/Glyph.hpp"

namespace Trident {
	struct FontAsset : public Asset {
		/// See https://developer.apple.com/fonts/TrueType-Reference-Manual/RM02/Chap2.html#converting.
		inline float32 calculateScale(float32 pointSize, uint32 dpi) const {
			return pointSize * dpi / (72 * unitsPerEm);
		}

		inline float32 pxToPt(float32 px, uint32 dpi) const {
			return px * 72 / dpi;
		}

		inline float32 ptToPx(float32 pointSize, uint32 dpi) const {
			return pointSize * dpi / 72;
		}

		// Returns the glyph represented by the provided code point.
		// If no corresponding glyph is found, the .notdef glyph is returned.
		inline const Glyph& operator[](uint16 codePoint) const {
			if (codePoint == 0) {
				return glyphs[0];
			}

			const auto glyphIndex = unicodeMap.find(codePoint);

			if (glyphIndex == unicodeMap.end()) {
				// Return .noglyph if we encounter a UTF-16 code point that's not supported by the font.
				return glyphs[0];
			}

			return glyphs[glyphIndex->second];
		}

		inline sint16 getKerning(uint16 firstGlyphIndex, uint16 secondGlyphIndex) const {
			const auto kerning = kerningPairs.find({firstGlyphIndex, secondGlyphIndex});

			if (kerning == kerningPairs.end()) {
				return 0;
			}

			return kerning->second;
		}

		ref<ImageAsset> glyphMapImage;
		sint16 ascent;
		sint32 lineHeight;
		uint16 unitsPerEm;
		sequence<Glyph> glyphs;

		/// Layout: <code point, glyph index>
		std::unordered_map<uint16, uint32> unicodeMap;

		/// Layout: <glyph index pair, first glyph X advance>
		std::unordered_map<GlyphPair, sint16> kerningPairs;
	};
}