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
		component.zIndex = parent.zIndex + style.zIndex;

		setComponentSize(component, style.minimumSize, style.maximumSize);

		component.padding = style.padding;
		component.anchorPoint = style.anchorPoint;
		component.layoutDirection = style.layoutDirection;
		component.layoutOffset = 0_px;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;

		// TODO(gui/component/text-input): Change to TEXT.
		component.pointerType = PointerType::INTERACTIVE;

		component.inputText = style.text;
		component.textComponent = false;
		component.debugName = style.debugName;

		openComponent(component, true);
	}

	void GUILayer::updateTextInput(KeyDownEvent& event, GUIContainer& focusedComponent) {
		DEBUG_ASSERT(focusedComponent.inputText != nullptr);

		std::string& text = *focusedComponent.inputText;

		if (event.getKeyCode() >= KeyCode::A && event.getKeyCode() <= KeyCode::Z) {
			const char8 character = static_cast<char8>(static_cast<uint16>(event.getKeyCode()));

			text += character;
		}

		if (event.getKeyCode() == KeyCode::BACKSPACE) {
			if (text.size() > 0) {
				text.pop_back();
			}
		}
	}
}