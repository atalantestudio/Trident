#include "pch.hpp"

#include "Engine/GUI/Object/GUIObject.hpp"

namespace ProjectA {
	GUIObject::GUIObject() {}

	const vec2<float32>& GUIObject::getPosition() const {
		return position;
	}

	void GUIObject::setPosition(const vec2<float32>& position) {
		this->position = position;
	}

	uint32 GUIObject::getColorIndex() const {
		return colorIndex;
	}

	void GUIObject::setColorIndex(uint32 colorIndex) {
		this->colorIndex = colorIndex;
	}

	uint32 GUIObject::getTextureIndex() const {
		return textureIndex;
	}

	void GUIObject::setTextureIndex(uint32 textureIndex) {
		this->textureIndex = textureIndex;
	}

	const chain<GUISubcomponent>& GUIObject::getSubcomponents() const {
		return subcomponents;
	}
}