// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Font/Glyph.hpp"

namespace Trident {
	struct FontAssetDescriptor : public Asset {
		ref<ImageAsset> glyphMapImage;
		sint16 ascent;
		sint32 lineHeight;
		uint16 unitsPerEm;
		sequence<Glyph> glyphs;
		std::unordered_map<uint16, uint32> unicodeMap;
		std::unordered_map<GlyphPair, sint16> kerningPairs;
	};

	struct FontAsset : public Asset {
		public:
			explicit FontAsset(const FontAssetDescriptor& descriptor) :
				Asset{descriptor.identifier, descriptor.version},
				glyphMapImage(descriptor.glyphMapImage),
				ascent(descriptor.ascent),
				lineHeight(descriptor.lineHeight),
				unitsPerEm(descriptor.unitsPerEm),
				glyphs(descriptor.glyphs),
				unicodeMap(descriptor.unicodeMap),
				kerningPairs(descriptor.kerningPairs)
			{}

			inline ref<ImageAsset> getGlyphMapImage() const {
				return glyphMapImage;
			}

			inline sint16 getAscent() const {
				return ascent;
			}

			inline sint32 getLineHeight() const {
				return lineHeight;
			}

			inline const sequence<Glyph>& getGlyphs() const {
				return glyphs;
			}

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

			inline uint32 getGlyphCount() const {
				return static_cast<uint32>(glyphs.count());
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

		private:
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