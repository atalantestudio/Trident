#include "pch.hpp"

#include "Engine/GUI/Font/BitmapFont.hpp"
#include "Engine/GUI/Object/GUITextObject.hpp"

namespace ProjectA {
	GUITextObject::GUITextObject(const std::string& text, const GUITextObjectDescriptor& descriptor) {
		setColorIndex(0);

		const BitmapFont& bitmapFont = *static_cast<const BitmapFont*>(descriptor.font.get());

		float32 xOffset = 0.0f;

		for (const char character : text) {
			const Glyph& glyph = bitmapFont.glyphMap.map[character - bitmapFont.glyphMap.base];

			GUISubcomponent subcomponent;
			subcomponent.offset = {xOffset, 0.0f};
			subcomponent.size = {glyph.width, bitmapFont.tileHeight};
			subcomponent.uv = glyph.textureCoordinates;

			subcomponents.emplace_back(subcomponent);

			xOffset += glyph.width + bitmapFont.tileSpacing;
		}
	}
}