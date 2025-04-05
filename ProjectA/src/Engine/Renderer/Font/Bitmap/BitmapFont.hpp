#pragma once

#include "Engine/Renderer/Font/Bitmap/BitmapGlyphMap.hpp"
#include "Engine/Renderer/Font/Font.hpp"

namespace ProjectA {
	class BitmapFont : public Font {
		public:
			uint32 textureIndex;
			BitmapGlyphMap glyphMap;
			float32 tileHeight;
			float32 tileSpacing;
	};
}