// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	AABB2D GUILayer::clipAabb(AABB2D a, AABB2D b) {
		const vec2<float32> s = a.max - a.min;

		AABB2D t{};
		t.min = {0.0f, 0.0f};
		t.max = {1.0f, 1.0f};

		if (a.min.x < b.min.x) {
			t.min.x = (b.min.x - a.min.x) / s.x;
		}

		if (a.min.y < b.min.y) {
			t.min.y = (b.min.y - a.min.y) / s.y;
		}

		if (a.max.x > b.max.x) {
			t.max.x = (b.max.x - a.min.x) / s.x;
		}

		if (a.max.y > b.max.y) {
			t.max.y = (b.max.y - a.min.y) / s.y;
		}

		return t;
	}

	bool GUILayer::registerComponent(const GUIComponent& component, uint32 componentIndex) {
		if (component.color.value.w == 0.0f) {
			return true;
		}

		AABB2D aabb{};
		aabb.min = component.origin;
		aabb.max = aabb.min + vec2<float32>{component.size.width.value, component.size.height.value};

		AABB2D t{};
		t.min = {0.0f, 0.0f};
		t.max = {1.0f, 1.0f};

		if (component.parentIndex == max<uint32>()) {
			aabb.max.y = static_cast<float32>(getApplication()->getWindow()->getExtent().y);
		}
		else {
			const GUIComponent& parent = components[component.parentIndex];

			AABB2D parentAabb{};
			parentAabb.min = parent.origin;
			parentAabb.max = parentAabb.min + vec2<float32>{parent.size.width.value, parent.size.height.value};

			switch (component.type) {
				case GUIComponentType::TEXT: {
					renderText(component, parentAabb);

					return false;
				}
				case GUIComponentType::TEXT_INPUT: {
					renderText(component, parentAabb);

					if (IsFocused(componentIndex)) {
						renderTextCursor(component);

						if (component.inputText->hasSelection()) {
							renderTextSelectionBox(component);
						}
					}

					return false;
				}
				default: {
					break;
				}
			}

			if (parent.type == GUIComponentType::VERTICAL_LIST) {
				t = clipAabb(aabb, parentAabb);
			}
		}

		if (t.min.x > 1.0f || t.min.y > 1.0f || t.max.x < 0.0f || t.max.y < 0.0f) {
			return false;
		}

		aabb.min = lerp(aabb.min, aabb.max, t.min);
		aabb.max = lerp(aabb.min, aabb.max, t.max);

		GUISubcomponent subcomponent{};
		subcomponent.worldTransform = (
			scaling<mat3x3<float32>>(aabb.max - aabb.min) *
			translation<mat3x3<float32>>(aabb.min)
		);
		subcomponent.textureTransform = (
			scaling<mat3x3<float32>>(t.max - t.min) *
			translation<mat3x3<float32>>(t.min)
		);
		subcomponent.color = component.color.value;
		subcomponent.zIndex = component.zIndex;
		subcomponent.textureArrayLayerIndex = component.image;

		renderer.registerSubcomponent(subcomponent);

		return true;
	}

	void GUILayer::renderText(const GUIComponent& component, AABB2D parentAabb) {
		const FontData& fontData = fonts[component.font];

		GUIGlyphSubcomponent glyphSubcomponent{};
		glyphSubcomponent.color = component.color.value;
		glyphSubcomponent.zIndex = component.zIndex;
		glyphSubcomponent.textureArrayLayerIndex = fontData.glyphMapImageId;
		glyphSubcomponent.fontSize = component.fontSize;

		for (const TextCharacter& character : component.characters) {
			AABB2D aabb{};
			aabb.min = component.origin + character.offset;
			aabb.max = aabb.min + character.size;

			const AABB2D t = clipAabb(aabb, parentAabb);

			if (t.min.x > 1.0f || t.min.y > 1.0f || t.max.x < 0.0f || t.max.y < 0.0f) {
				continue;
			}

			aabb.min = lerp(aabb.min, aabb.max, t.min);
			aabb.max = lerp(aabb.min, aabb.max, t.max);

			glyphSubcomponent.world = (
				scaling<mat3x3<float32>>(aabb.max - aabb.min) *
				translation<mat3x3<float32>>(aabb.min)
			);

			aabb.min = lerp(character.startTextureCoordinate, character.endTextureCoordinate, t.min);
			aabb.max = lerp(character.startTextureCoordinate, character.endTextureCoordinate, t.max);

			glyphSubcomponent.textureWorld = (
				scaling<mat3x3<float32>>(aabb.max - aabb.min) *
				translation<mat3x3<float32>>(aabb.min)
			);

			renderer.registerGlyphSubcomponent(glyphSubcomponent);
		}
	}

	void GUILayer::renderTextCursor(const GUIComponent& component) {
		const vec2<float32> offset = {component.padding.x + textCursorHorizontalOffset, component.padding.y};
		const vec2<float32> size = {TEXT_CURSOR_WIDTH, component.size.height.value - component.padding.w - component.padding.y};

		GUISubcomponent subcomponent{};
		subcomponent.worldTransform = (
			scaling<mat3x3<float32>>(size) *
			translation<mat3x3<float32>>(component.origin + offset)
		);
		subcomponent.textureTransform = identity<mat3x3<float32>>();
		subcomponent.color = TEXT_CURSOR_COLOR.value;
		subcomponent.zIndex = component.zIndex + 1;
		subcomponent.textureArrayLayerIndex = 0;

		renderer.registerSubcomponent(subcomponent);
	}

	void GUILayer::renderTextSelectionBox(const GUIComponent& component) {
		float32 startOffset = textSelectionBoxHorizontalOffset;
		float32 endOffset = textCursorHorizontalOffset;

		if (startOffset > endOffset) {
			startOffset = textCursorHorizontalOffset;
			endOffset = textSelectionBoxHorizontalOffset;
		}

		const vec2<float32> offset = {component.padding.x + startOffset, component.padding.y};
		const vec2<float32> size = {endOffset - startOffset, component.size.height.value - component.padding.w - component.padding.y};

		GUISubcomponent subcomponent{};
		subcomponent.worldTransform = (
			scaling<mat3x3<float32>>(size) *
			translation<mat3x3<float32>>(component.origin + offset)
		);
		subcomponent.textureTransform = identity<mat3x3<float32>>();
		subcomponent.color = TEXT_SELECTION_BOX_COLOR.value;
		subcomponent.zIndex = component.zIndex;
		subcomponent.textureArrayLayerIndex = 0;

		renderer.registerSubcomponent(subcomponent);
	}
}