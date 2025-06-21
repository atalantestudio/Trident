#pragma once

#include "Engine/GUI/Font/Bitmap/BitmapGlyphMap.hpp"
#include "Engine/GUI/Font/Font.hpp"

namespace ProjectA {
	class BitmapFont : public Font {
		public:
			uint32 textureIndex;
			BitmapGlyphMap glyphMap;
			float32 tileHeight;
			float32 tileSpacing;
	};
}