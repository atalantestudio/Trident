// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Font/Glyph.hpp"

namespace Trident {
	struct FontAssetDescriptor : public AssetDescriptor {
		ref<ImageAsset> glyphMapImage;
		sint32 lineHeight;
		sequence<Glyph> glyphs;
		std::unordered_map<uint16, uint32> unicodeMap;
	};

	// TODO: FontAssetHeader structure.

	class FontAsset : public Asset {
		public:
			explicit FontAsset(const FontAssetDescriptor& descriptor) :
				Asset(descriptor),
				glyphMapImage(descriptor.glyphMapImage),
				lineHeight(descriptor.lineHeight),
				glyphs(descriptor.glyphs),
				unicodeMap(descriptor.unicodeMap)
			{}

			ref<ImageAsset> getGlyphMapImage() const {
				return glyphMapImage;
			}

			sint32 getLineHeight() const {
				return lineHeight;
			}

			const sequence<Glyph>& getGlyphs() const {
				return glyphs;
			}

			uint32 getGlyphCount() const {
				return static_cast<uint32>(glyphs.count());
			}

			// Returns the glyph represented by the provided code point.
			// If no corresponding glyph is found, the .notdef glyph is returned.
			const Glyph& operator [](uint16 codePoint) const {
				if (codePoint == 0) {
					return glyphs[0];
				}

				const auto glyphIndex = unicodeMap.find(codePoint);

				if (glyphIndex == unicodeMap.end()) {
					// Encountered a UTF-16 code point that's not supported by the font.
					return glyphs[0];
				}

				return glyphs[glyphIndex->second];
			}

		private:
			ref<ImageAsset> glyphMapImage;
			sint32 lineHeight;
			sequence<Glyph> glyphs;

			// <code point, glyph index>
			std::unordered_map<uint16, uint32> unicodeMap;
	};
}