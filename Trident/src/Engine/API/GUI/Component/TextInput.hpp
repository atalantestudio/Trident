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

	void GUILayer::updateTextInput(TextInputEvent& event, GUIContainer& component) {
		DEBUG_ASSERT(component.inputText != nullptr);

		component.inputText->insert(event.characterCode);
	}

	void GUILayer::updateTextInputAction(KeyDownEvent& event, GUIContainer& component) {
		DEBUG_ASSERT(component.inputText != nullptr);

		switch (event.getKeyCode()) {
			case KeyCode::BACKSPACE: {
				if (component.inputText->getByteSize() == 0) {
					break;
				}

				component.inputText->markCursorForSelection();
				component.inputText->moveCursor(-1);
				component.inputText->removeSelection();

				break;
			}
		}
	}
}