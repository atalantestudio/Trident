#pragma once

#include "Engine/Renderer/Font/Font.hpp"
#include "Engine/Renderer/Font/GlyphMap.hpp"

namespace ProjectA {
	struct BitmapFont : public Font {
		uint32 textureIndex;
		GlyphMap glyphMap;
		float32 tileHeight;
		float32 tileSpacing;
	};
}