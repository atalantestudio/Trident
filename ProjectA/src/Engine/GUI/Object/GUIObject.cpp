#include "pch.hpp"

#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/GUI/Object/GUIObject.hpp"

namespace ProjectA {
	GUIObject::GUIObject() {}

	void GUIObject::setLayoutDistribution(LayoutDistribution layoutDistribution) {
		this->layoutDistribution = layoutDistribution;
	}

	void GUIObject::setFillMode(FillMode fillMode) {
		this->fillMode = fillMode;
	}

	void GUIObject::setDefaultSize(const vec2<float32>& defaultSize) {
		this->defaultSize = defaultSize;
	}

	const vec2<float32>& GUIObject::getPosition() const {
		return position;
	}

	void GUIObject::setPosition(const vec2<float32>& position) {
		this->position = position;
	}

	void GUIObject::setMargin(const vec2<float32>& margin) {
		this->margin = margin;
	}

	void GUIObject::setSpacing(const vec2<float32>& spacing) {
		this->spacing = spacing;
	}

	void GUIObject::setColor(const vec4<float32>& color) {
		this->color= color;
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

	void GUIObject::compute(ECS& ecs, const vec2<float32>& parentSize, const vec2<float32>& parentMargin, chain<vec4<float32>>& colors) {
		float32 width;

		if (fillMode & FillMode::EXPAND_HORIZONTAL) {
			width = parentSize[0];
		}
		else {
			width = defaultSize[0];
		}

		float32 height;

		if (fillMode & FillMode::EXPAND_VERTICAL) {
			height = parentSize[1];
		}
		else {
			height = defaultSize[1];
		}

		width -= parentMargin[0] * 2;
		height -= parentMargin[1] * 2;

		GUISubcomponent subcomponent;
		subcomponent.offset = parentMargin;
		subcomponent.size = {width, height};
		subcomponent.uv = {0.0f, 0.0f};

		subcomponents.emplace_back(subcomponent);

		for (const GUISubcomponent& subcomponent : subcomponents) {
			const Entity entity = ecs.createEntity();

			GUIComponent component;
			component.transform = mat3x3<float32>::scaling(subcomponent.size) * mat3x3<float32>::translation(position + subcomponent.offset);
			component.colorIndex = static_cast<uint32>(colors.size());
			component.textureIndex = textureIndex;

			ecs.setComponent<GUIComponent>(entity, component);
		}

		colors.emplace_back(color);

		for (const ref<GUIObject>& object : objects) {
			object->compute(ecs, defaultSize, margin, colors);
		}
	}
}