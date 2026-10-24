// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::TextInput(const TextInputStyle& style) {
		const uint32 componentIndex = componentCount;
		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];

		GUIContainer& parent = components[parentIndex];

		parent.childIndices.emplace_back(componentIndex);

		const FontData& fontData = fonts[style.font];
		const FontAsset& fontAsset = *fontData.fontAsset;
		const uint32 dpi = getApplication()->getWindow()->getDpi();
		const float32 fontSizePt = fontAsset.pxToPt(style.fontSize, dpi);
		const float32 scale = fontAsset.calculateScale(fontSizePt, dpi);
		const float32 lineHeight = fontAsset.lineHeight * scale;

		GUIContainer component{};
		component.type = GUIComponentType::TEXT_INPUT;
		component.parentIndex = parentIndex;
		component.floating = false;
		component.origin = parent.origin;
		component.offset = {0_px, 0_px};
		component.size = {style.width, lineHeight};
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
		component.font = style.font;
		component.fontSize = style.fontSize;
		component.debugName = style.debugName;

		setComponentSize(component, style.minimumSize, style.maximumSize);

		components[componentCount] = component;
		focusableComponentIndexToComponentIndex[focusableComponentCount] = componentIndex;

		componentCount += 1;
		focusableComponentCount += 1;

		if (previousComponentCount >= componentCount) {
			const GUIContainer& component = previousComponents[componentCount - 1];
			const Window* window = getApplication()->getWindow();
			const vec2<float32> mousePosition = vec2<float32>(window->getMousePosition());

			if (IsIntersecting(component, mousePosition)) {
				window->updatePointerType(component.pointerType);
			}
		}

		AdjustWidth(componentIndex);
	}

	void GUILayer::WrapInputText(GUIContainer& component, uint32 componentIndex) {
		const bool focused = IsFocused(componentIndex);
		const ref<FontAsset> fontAsset = fonts[component.font].fontAsset;
		const float32 fontScale = calculateFontScale(fontAsset, component.fontSize);
		const UTF8String* inputText = component.inputText;

		if (focused) {
			textCursorHorizontalOffset = 0_px;
			textSelectionBoxHorizontalOffset = 0_px;
		}

		vec2<float32> offset = component.origin + vec2<float32>{component.padding.x, component.padding.y + fontAsset->ascent * fontScale};
		UTF8Stream stream = UTF8Stream(view<char8>(*inputText));

		while (stream.getRemainingByteSize() > 0) {
			const uint32 codePoint = stream.read();

			DEBUG_ASSERT(codePoint != '\n');

			const uint64 byteOffset = stream.getByteOffset();
			const Glyph& glyph = fontAsset->operator[](codePoint);

			if (!glyph.whitespace) {
				// TODO(gui/text/kerning)
				/* if (stream.getRemainingByteSize() > 0) {
					const uint32 nextCodePoint = stream.peek();
					const sint16 kerning = fontAsset->getKerning(codePoint, nextCodePoint);

					offset.x += kerning * fontScale;
				} */

				const TextCharacter character = createTextCharacter(glyph, offset, fontScale);

				component.characters.emplace_back(character);
			}

			offset.x += glyph.advanceWidth * fontScale;

			if (focused) {
				if (inputText->cursorOffset == byteOffset) {
					textCursorHorizontalOffset = offset.x;
				}

				if (inputText->hasSelection() && inputText->selectionMarkerOffset == byteOffset) {
					textSelectionBoxHorizontalOffset = offset.x;
				}
			}
		}
	}

	void GUILayer::updateTextInput(TextInputEvent& event, const GUIContainer& component) const {
		UTF8String* inputText = component.inputText;

		DEBUG_ASSERT(inputText != nullptr);

		inputText->insert(event.characterCode);
	}

	void GUILayer::updateTextInputAction(KeyDownEvent& event, const GUIContainer& component) const {
		UTF8String* inputText = component.inputText;

		DEBUG_ASSERT(inputText != nullptr);

		const Window* window = getApplication()->getWindow();

		// TODO(gui/component/text-input/seek-current-word-start): Seek the current word start when Ctrl + ArrowLeft is pressed.
		// TODO(gui/component/text-input/seek-current-word-end): Seek the current word end when Ctrl + ArrowRight is pressed.
		// TODO(gui/component/text-input/select-current-word): When Ctrl + Shift + ArrowLeft/ArrowRight is pressed, seek the current word start/end while selecting.
		// TODO(gui/component/text-input/ctrl-backspace): Implement Ctrl + Backspace key combination (delete left until not in [A-Za-z0-9_]).
		// TODO(gui/component/text-input/ctrl-delete): Implement Ctrl + Delete key combination (delete right until not in [A-Za-z0-9_]).
		// TODO(gui/console/parse-command): Parse the command options when Enter is pressed, then trigger a ConsoleCommandEvent.
		// TODO(gui/console/inline-suggestions): Implement Tab key to accept inline suggestions?
		switch (event.getKeyCode()) {
			case KeyCode::BACKSPACE: {
				if (inputText->getByteSize() == 0) {
					break;
				}

				if (!inputText->hasSelection()) {
					inputText->select();
					inputText->jumpLeft(1);
				}

				inputText->removeSelection();

				break;
			}
			case KeyCode::SHIFT: {
				if (!inputText->hasSelection()) {
					inputText->select();
				}

				break;
			}
			case KeyCode::ESCAPE: {
				if (inputText->hasSelection()) {
					inputText->unselect();
				}

				break;
			}
			case KeyCode::ARROW_LEFT: {
				if (!window->isKeyDown(KeyCode::SHIFT) && inputText->hasSelection()) {
					inputText->unselect();
					inputText->seek(min(inputText->cursorOffset, inputText->selectionMarkerOffset));

					break;
				}

				if (inputText->cursorOffset == 0) {
					break;
				}

				inputText->jumpLeft(1);

				break;
			}
			case KeyCode::HOME: {
				if (!window->isKeyDown(KeyCode::SHIFT) && inputText->hasSelection()) {
					inputText->unselect();
				}

				inputText->seek(0);

				break;
			}
			case KeyCode::ARROW_RIGHT: {
				if (!window->isKeyDown(KeyCode::SHIFT) && inputText->hasSelection()) {
					inputText->unselect();
					inputText->seek(max(inputText->cursorOffset, inputText->selectionMarkerOffset));

					break;
				}

				if (inputText->cursorOffset >= inputText->getByteSize()) {
					break;
				}

				inputText->jumpRight(1);

				break;
			}
			case KeyCode::END: {
				if (!window->isKeyDown(KeyCode::SHIFT) && inputText->hasSelection()) {
					inputText->unselect();
				}

				inputText->seek(inputText->getByteSize());

				break;
			}
			case KeyCode::DELETE: {
				if (inputText->cursorOffset >= inputText->getByteSize()) {
					break;
				}

				if (!inputText->hasSelection()) {
					inputText->jumpRight(1);
					inputText->select();
					inputText->jumpLeft(1);
				}

				inputText->removeSelection();

				break;
			}
			case KeyCode::A: {
				if (!window->isKeyDown(KeyCode::CONTROL)) {
					break;
				}

				inputText->seek(0);
				inputText->select();
				inputText->seek(inputText->getByteSize());

				break;
			}
			case KeyCode::C: {
				if (!window->isKeyDown(KeyCode::CONTROL)) {
					break;
				}

				if (!inputText->hasSelection()) {
					break;
				}

				window->copyToClipboard(inputText->getSelection());

				break;
			}
			case KeyCode::V: {
				if (!window->isKeyDown(KeyCode::CONTROL)) {
					break;
				}

				if (!window->textInClipboard()) {
					break;
				}

				const sequence<char8> text = window->getClipboardText();
				const view<char8> textView(&text[0], text.count() - 1);

				inputText->insert(textView);

				break;
			}
			case KeyCode::X: {
				if (!window->isKeyDown(KeyCode::CONTROL)) {
					break;
				}

				if (!inputText->hasSelection()) {
					break;
				}

				window->copyToClipboard(inputText->getSelection());

				inputText->removeSelection();

				break;
			}
			default: {
				break;
			}
		}
	}
}