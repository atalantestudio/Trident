#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Font/Glyph.hpp"

namespace ProjectA {
	struct FontDescriptor {
		ref<ImageAsset> glyphMapImage;
		sequence<Glyph> glyphs;
		std::unordered_map<uint16, uint32> unicodeMap;
		sint32 lineHeight;
	};

	class Font {
		public:
			explicit Font(const FontDescriptor& descriptor) :
				glyphMapImage(descriptor.glyphMapImage),
				glyphs(descriptor.glyphs),
				unicodeMap(descriptor.unicodeMap),
				lineHeight(descriptor.lineHeight)
			{}

			const ref<ImageAsset>& getGlyphMapImage() const {
				return glyphMapImage;
			}

			const sequence<Glyph>& getGlyphs() const {
				return glyphs;
			}

			uint32 getGlyphCount() const {
				return static_cast<uint32>(glyphs.count);
			}

			sint32 getLineHeight() const {
				return lineHeight;
			}

			// Returns the glyph represented by the provided code point.
			// TODO: If no corresponding glyph is found, the .notdef glyph is returned.
			const Glyph& operator [](uint16 codePoint) const {
				const std::list<std::pair<const uint16, uint32>, std::allocator<std::pair<const uint16, uint32>>>::const_iterator glyphIndex = unicodeMap.find(codePoint);

				return glyphs[glyphIndex->second];
			}

		private:
			ref<ImageAsset> glyphMapImage;
			sequence<Glyph> glyphs;
			std::unordered_map<uint16, uint32> unicodeMap;
			sint32 lineHeight;
	};
}