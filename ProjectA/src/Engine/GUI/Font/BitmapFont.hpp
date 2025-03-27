#pragma once

#include "Engine/GUI/Font/Font.hpp"
#include "Engine/GUI/Font/GlyphMap.hpp"

namespace ProjectA {
	struct BitmapFont : public Font {
		uint32 textureIndex;
		GlyphMap glyphMap;
		float32 tileHeight;
		float32 tileSpacing;
	};
}