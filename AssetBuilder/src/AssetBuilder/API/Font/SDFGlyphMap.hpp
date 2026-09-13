// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct SDFGlyphMap {
		sequence<uint8> data;
		Extent2D extent;
		sequence<Glyph> glyphs;
	};
}