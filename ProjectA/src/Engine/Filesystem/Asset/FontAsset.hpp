#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Font/Glyph.hpp"

namespace ProjectA {
	struct FontAssetDescriptor : public AssetDescriptor {
		ref<ImageAsset> glyphMapImage;
		sint32 lineHeight;
		sequence<Glyph> glyphs;
		std::unordered_map<uint16, uint32> unicodeMap;
	};

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

				const std::list<std::pair<const uint16, uint32>, std::allocator<std::pair<const uint16, uint32>>>::const_iterator glyphIndex = unicodeMap.find(codePoint);

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