#pragma once

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/GUI/Font/Font.hpp"
#include "Engine/GUI/Font/Glyph.hpp"

namespace ProjectA {
	class Font {
		public:
			explicit Font(const File<FileFormat::TTF>& file);

			const ref<ImageAsset>& getGlyphMapImage() const;

			const sequence<Glyph>& getGlyphMap() const;

			uint32 getGlyphCount() const;

			sint32 getLineHeight() const;

			// Returns the glyph represented by the provided code point.
			// TODO: If no corresponding glyph is found, the .notdef glyph is returned.
			const Glyph& operator [](uint16 codePoint) const;

		private:
			ref<ImageAsset> glyphMapImage;
			sequence<Glyph> glyphMap;
			std::unordered_map<uint16, uint32> unicodeMap;
			sint32 lineHeight;
	};
}