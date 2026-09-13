// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Font/Glyph.hpp"

namespace Trident {
	struct BuildFont {
		ref<ImageAsset> glyphMapImage;
		sint16 ascent;
		sint16 descent;
		sint16 lineGap;
		uint16 unitsPerEm;
		sequence<Glyph> glyphs;
		std::unordered_map<uint16, uint32> unicodeMap;
		std::unordered_map<GlyphPair, sint16> kerningPairs;
	};
}