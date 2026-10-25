// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	bool GUILayer::registerComponent(const GUIComponent& component, uint32 componentIndex) {
		if (component.color.value.w == 0.0f) {
			return true;
		}

		vec2<float32> size = {component.size.width.value, component.size.height.value};

		if (component.parentIndex != max<uint32>()) {
			switch (component.type) {
				case GUIComponentType::TEXT: {
					renderText(component);

					return false;
				}
				case GUIComponentType::TEXT_INPUT: {
					renderText(component);

					if (IsFocused(componentIndex)) {
						renderTextCursor(component);

						if (component.inputText->hasSelection()) {
							renderTextSelectionBox(component);
						}
					}

					return false;
				}
				default: {
					const GUIComponent& parent = components[component.parentIndex];

					if (parent.type != GUIComponentType::VERTICAL_LIST) {
						break;
					}

					if (component.origin.y + component.size.height.value <= parent.origin.y) {
						return false;
					}

					if (component.origin.y >= parent.origin.y + parent.size.height.value) {
						return false;
					}

					const float32 topClip = (component.origin.y + component.size.height.value) - parent.origin.y;

					// TODO(gui/scroll-container/clipping): Finish clipping.
					/* if (topClip < component.origin.y) {
						y = topClip;
					} */

					const float32 bottomClip = (parent.origin.y + parent.size.height.value) - component.origin.y;

					if (bottomClip < size.y) {
						size.y = bottomClip;
					}

					break;
				}
			}
		}
		else {
			size.y = static_cast<float32>(getApplication()->getWindow()->getExtent().y);
		}

		GUISubcomponent subcomponent{};
		subcomponent.worldTransform = (
			scaling<mat3x3<float32>>(size) *
			translation<mat3x3<float32>>(component.origin)
		);
		subcomponent.textureTransform = identity<mat3x3<float32>>();
		subcomponent.color = component.color.value;
		subcomponent.zIndex = component.zIndex;
		subcomponent.textureArrayLayerIndex = component.image;

		renderer.registerSubcomponent(subcomponent);

		return true;
	}

	void GUILayer::renderText(const GUIComponent& component) {
		const FontData& fontData = fonts[component.font];

		GUIGlyphSubcomponent glyphSubcomponent{};
		glyphSubcomponent.color = component.color.value;
		glyphSubcomponent.zIndex = component.zIndex;
		glyphSubcomponent.textureArrayLayerIndex = fontData.glyphMapImageId;
		glyphSubcomponent.fontSize = component.fontSize;

		for (const TextCharacter& character : component.characters) {
			glyphSubcomponent.world = (
				scaling<mat3x3<float32>>(character.size) *
				translation<mat3x3<float32>>(component.origin + hinting + character.offset)
			);
			glyphSubcomponent.textureWorld = (
				scaling<mat3x3<float32>>(character.endTextureCoordinate - character.startTextureCoordinate) *
				translation<mat3x3<float32>>(character.startTextureCoordinate)
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