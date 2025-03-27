#include "pch.hpp"

#include "Engine/GUI/Object/GUIObject.hpp"

namespace ProjectA {
	GUIObject::GUIObject(const GUIObjectDescriptor& descriptor) :
		position(descriptor.position),
		colorIndex(descriptor.colorIndex),
		textureIndex(descriptor.textureIndex)
	{
		GUISubcomponent subcomponent;
		subcomponent.offset = {0.0f, 0.0f};
		subcomponent.size = descriptor.size;
		subcomponent.uv = {0.0f, 0.0f};

		subcomponents.emplace_back(subcomponent);
	}

	const vec2<float32>& GUIObject::getPosition() const {
		return position;
	}

	uint32 GUIObject::getColorIndex() const {
		return colorIndex;
	}

	uint32 GUIObject::getTextureIndex() const {
		return textureIndex;
	}

	const chain<GUISubcomponent>& GUIObject::getSubcomponents() const {
		return subcomponents;
	}
}