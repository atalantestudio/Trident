#include "pch.hpp"

#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/GUI/Object/GUITextObject.hpp"
#include "Engine/Renderer/Font/Bitmap/BitmapFont.hpp"

namespace ProjectA {
	GUITextObject::GUITextObject(const std::string& text) :
		text(text)
	{}

	void GUITextObject::compute(ECS& ecs, const vec2<float32>& parentSize, const vec2<float32>& parentMargin, chain<vec4<float32>>& colors, const ref<Font>& font) {
		const BitmapFont& bitmapFont = *std::static_pointer_cast<BitmapFont>(font);

		float32 xOffset = parentMargin[0];

		for (const char character : text) {
			const BitmapGlyph& glyph = bitmapFont.glyphMap.map[character - bitmapFont.glyphMap.base];

			GUISubcomponent subcomponent;
			subcomponent.offset = {xOffset, parentMargin[1]};
			subcomponent.size = {glyph.width, bitmapFont.tileHeight};
			subcomponent.textureCoordinates = glyph.textureCoordinates;

			subcomponents.emplace_back(subcomponent);

			xOffset += glyph.width + bitmapFont.tileSpacing;

			const Entity entity = ecs.createEntity();

			GUIComponent component;
			component.transform = mat3x3<float32>::scaling(subcomponent.size) * mat3x3<float32>::translation(position + subcomponent.offset);
			component.colorIndex = 0;
			component.textureIndex = 0;

			ecs.setComponent<GUIComponent>(entity, component);
		}

		for (const ref<GUIObject>& object : objects) {
			object->compute(ecs, defaultSize, parentMargin + margin, colors, font);
		}
	}
}