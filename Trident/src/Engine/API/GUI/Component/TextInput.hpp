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

		// TODO(gui/component/text-input): Restore pointer type update?
		/* if (previousComponentCount >= componentCount) {
			const GUIContainer& component = previousComponents[componentCount - 1];
			const Window* window = getApplication()->getWindow();
			const vec2<float32> mousePosition = vec2<float32>(window->getMousePosition());

			if (IsIntersecting(component, mousePosition)) {
				window->updatePointerType(component.pointerType);
			}
		} */

		AdjustWidth(componentIndex);
	}

	void GUILayer::WrapInputText(GUIContainer& component, uint32 componentIndex) {
		const FontData& fontData = fonts[component.font];
		const FontAsset& fontAsset = *fontData.fontAsset;
		const uint32 dpi = getApplication()->getWindow()->getDpi();
		const float32 fontSizePt = fontAsset.pxToPt(component.fontSize, dpi);
		const float32 scale = fontAsset.calculateScale(fontSizePt, dpi);

		const UTF8String* inputText = component.inputText;

		UTF8Stream stream = UTF8Stream(view<char8>(*inputText));
		float32 baseline = component.origin.y + fontAsset.ascent * scale;
		constexpr float32 horizontalHinting = 0.0f;
		float32 characterOffsetX = component.origin.x;

		const float32 baselineFraction = baseline - static_cast<sint32>(baseline);

		if (baselineFraction > 0.5f) {
			baseline = floor(baseline);
		}

		const bool focused = IsFocused(componentIndex);

		if (focused) {
			textCursorHorizontalOffset = 0_px;
			textSelectionBoxHorizontalOffset = 0_px;
		}

		while (stream.getRemainingByteSize() > 0) {
			const uint32 codePoint = stream.read();
			const uint64 byteOffset = stream.getByteOffset();

			DEBUG_ASSERT(codePoint != '\n');

			const Glyph& glyph = fontAsset[codePoint];

			if (glyph.whitespace) {
				characterOffsetX += glyph.advanceWidth * scale;
			}
			else {
				const float32 x = component.padding.x + horizontalHinting + ceil(characterOffsetX + glyph.leftSideBearing * scale);
				const float32 y = component.padding.y + baseline - glyph.yMax * scale;

				// TODO(gui/text/kerning)
				/* if (stream.getRemainingByteSize() > 0) {
					const uint32 nextCodePoint = stream.peek();
					const sint16 kerning = fontAsset.getKerning(codePoint, nextCodePoint);

					x += kerning * scale;
				} */

				const float32 width = glyph.width * scale;
				const float32 height = glyph.height * scale;

				TextCharacter character{};
				character.offset = {x, y};
				character.size = {width, height};
				character.startTextureCoordinate = glyph.minCoordinate;
				character.endTextureCoordinate = glyph.maxCoordinate;

				component.characters.emplace_back(character);

				characterOffsetX += glyph.advanceWidth * scale;
			}

			if (focused) {
				if (inputText->cursorOffset == byteOffset) {
					textCursorHorizontalOffset = characterOffsetX;
				}

				if (inputText->hasSelection() && inputText->selectionMarkerOffset == byteOffset) {
					textSelectionBoxHorizontalOffset = characterOffsetX;
				}
			}
		}
	}

	void GUILayer::updateTextInput(TextInputEvent& event, const GUIContainer& component) const {
		UTF8String* string = component.inputText;

		DEBUG_ASSERT(string != nullptr);

		string->insert(event.characterCode);
	}

	void GUILayer::updateTextInputAction(KeyDownEvent& event, const GUIContainer& component) const {
		const Window* window = getApplication()->getWindow();

		UTF8String* string = component.inputText;

		DEBUG_ASSERT(string != nullptr);

		// TODO(gui/component/text-input/seek-current-word-start): Seek the current word start when Ctrl + ArrowLeft is pressed.
		// TODO(gui/component/text-input/seek-current-word-end): Seek the current word end when Ctrl + ArrowRight is pressed.
		// TODO(gui/component/text-input/select-current-word): When Ctrl + Shift + ArrowLeft/ArrowRight is pressed, seek the current word start/end while selecting.
		// TODO(gui/component/text-input/ctrl-backspace): Implement Ctrl + Backspace key combination (delete left until not in [A-Za-z0-9_]).
		// TODO(gui/component/text-input/ctrl-delete): Implement Ctrl + Delete key combination (delete right until not in [A-Za-z0-9_]).
		// TODO(gui/console/inline-suggestions): Implement Tab key to accept inline suggestions?
		// TODO(gui/console/parse-command): Parse the command options when Enter is pressed, then trigger a ConsoleCommandEvent.
		switch (event.getKeyCode()) {
			case KeyCode::BACKSPACE: {
				if (string->getByteSize() == 0) {
					break;
				}

				if (!string->hasSelection()) {
					string->select();
					string->jumpLeft(1);
				}

				string->removeSelection();

				break;
			}
			case KeyCode::SHIFT: {
				if (!string->hasSelection()) {
					string->select();
				}

				break;
			}
			case KeyCode::ESCAPE: {
				if (string->hasSelection()) {
					string->unselect();
				}

				break;
			}
			case KeyCode::ARROW_LEFT: {
				if (!window->isKeyDown(KeyCode::SHIFT) && string->hasSelection()) {
					string->unselect();
					string->seek(min(string->cursorOffset, string->selectionMarkerOffset));

					break;
				}

				if (string->cursorOffset == 0) {
					break;
				}

				string->jumpLeft(1);

				break;
			}
			case KeyCode::HOME: {
				if (!window->isKeyDown(KeyCode::SHIFT) && string->hasSelection()) {
					string->unselect();
				}

				string->seek(0);

				break;
			}
			case KeyCode::ARROW_RIGHT: {
				if (!window->isKeyDown(KeyCode::SHIFT) && string->hasSelection()) {
					string->unselect();
					string->seek(max(string->cursorOffset, string->selectionMarkerOffset));

					break;
				}

				if (string->cursorOffset >= string->getByteSize()) {
					break;
				}

				string->jumpRight(1);

				break;
			}
			case KeyCode::END: {
				if (!window->isKeyDown(KeyCode::SHIFT) && string->hasSelection()) {
					string->unselect();
				}

				string->seek(string->getByteSize());

				break;
			}
			case KeyCode::DELETE: {
				if (string->cursorOffset >= string->getByteSize()) {
					break;
				}

				if (!string->hasSelection()) {
					string->jumpRight(1);
					string->select();
					string->jumpLeft(1);
				}

				string->removeSelection();

				break;
			}
			case KeyCode::A: {
				if (!window->isKeyDown(KeyCode::CONTROL)) {
					break;
				}

				string->seek(0);
				string->select();
				string->seek(string->getByteSize());

				break;
			}
			case KeyCode::C: {
				if (!window->isKeyDown(KeyCode::CONTROL)) {
					break;
				}

				if (!string->hasSelection()) {
					break;
				}

				window->copyToClipboard(string->getSelection());

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

				string->insert(textView);

				break;
			}
			case KeyCode::X: {
				if (!window->isKeyDown(KeyCode::CONTROL)) {
					break;
				}

				if (!string->hasSelection()) {
					break;
				}

				window->copyToClipboard(string->getSelection());

				string->removeSelection();

				break;
			}
			default: {
				break;
			}
		}
	}
}