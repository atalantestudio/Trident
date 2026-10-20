// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::openTextInput(const TextInputStyle& style) {
		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];
		const GUIContainer& parent = components[parentIndex];

		GUIContainer component{};
		component.type = GUIComponentType::TEXT_INPUT;
		component.parentIndex = parentIndex;
		component.floating = false;
		component.origin = parent.origin;
		component.offset = {0_px, 0_px};
		component.size = style.size;

		setComponentSize(component, style.minimumSize, style.maximumSize);

		component.zIndex = parent.zIndex + style.zIndex;
		component.padding = style.padding;
		component.anchorPoint = style.anchorPoint;
		component.layoutDirection = style.layoutDirection;
		component.layoutOffset = nullptr;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;
		component.pointerType = PointerType::TEXT;
		component.inputText = style.text;
		component.textComponent = false;
		component.debugName = style.debugName;

		openComponent(component, true);
	}

	void GUILayer::updateTextInput(TextInputEvent& event, GUIContainer& focusedComponent) {
		DEBUG_ASSERT(focusedComponent.inputText != nullptr);

		std::string& text = *focusedComponent.inputText;
		uint8 byteIndex = 0;

		while (byteIndex < event.byteSize) {
			const uint8 byte = static_cast<uint8>((event.characterCode >> (8 * byteIndex)) & 0xff);

			text += byte;
			byteIndex += 1;
		}

		// TODO(gui/text-input/backspace)
		/* if (text.size() > 0) {
			sint32 byteIndex = static_cast<sint32>(text.size()) - 1;

			while (byteIndex >= 0) {
				if (((text[byteIndex] >> 6) & 0b11) != 0b10) {
					break;
				}

				byteIndex -= 1;
			}

			text.erase(byteIndex);
		} */
	}
}