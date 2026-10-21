// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::openTextInput(const TextInputStyle& style) {
		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];
		const GUIContainer& parent = components[parentIndex];
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
		openComponent(component, true);

		Floating({
			.offset = {0.0f, 0.0f},
			.size = {0.0f, lineHeight},
			.zIndex = component.zIndex,
			.color = component.color,
		});
	}

	void GUILayer::WrapInputText(GUIContainer& component, GUIContainer& selectionComponent) {
		const FontData& fontData = fonts[component.font];
		const FontAsset& fontAsset = *fontData.fontAsset;
		const uint32 dpi = getApplication()->getWindow()->getDpi();
		const float32 fontSizePt = fontAsset.pxToPt(component.fontSize, dpi);
		const float32 scale = fontAsset.calculateScale(fontSizePt, dpi);

		UTF8Stream stream(view<char8>(*component.inputText));
		float32 baseline = component.origin.y + fontAsset.ascent * scale;
		constexpr float32 horizontalHinting = 0.0f;
		float32 characterOffsetX = component.origin.x;

		const float32 baselineFraction = baseline - static_cast<sint32>(baseline);

		if (baselineFraction > 0.5f) {
			baseline = floor(baseline);
		}

		uint64 byteOffset = 0;

		while (stream.getRemainingByteSize() > 0) {
			const uint64 previousByteOffset = stream.getByteOffset();
			const uint32 codePoint = stream.read();

			byteOffset = stream.getByteOffset();

			DEBUG_ASSERT(codePoint != '\n');

			const Glyph& glyph = fontAsset[codePoint];

			if (glyph.whitespace) {
				characterOffsetX += glyph.advanceWidth * scale;

				continue;
			}

			const float32 x = component.padding.x + horizontalHinting + ceil(characterOffsetX + glyph.leftSideBearing * scale);
			const float32 y = component.padding.y + baseline - glyph.yMax * scale;

			// TODO(gui/text/kerning)
			/* if (data.stream.getRemainingByteSize() > 0) {
				const uint32 nextCodePoint = data.stream.peek();
				const sint16 kerning = data.fontAsset->getKerning(codePoint, nextCodePoint);

				x += kerning * scale;
			} */

			const float32 width = glyph.width * scale;
			const float32 height = glyph.height * scale;

			TextCharacter textCharacter{};
			textCharacter.offset = {x, y};
			textCharacter.size = {width, height};
			textCharacter.startTextureCoordinate = glyph.minCoordinate;
			textCharacter.endTextureCoordinate = glyph.maxCoordinate;

			component.characters.emplace_back(textCharacter);

			if (component.inputText->hasSelection()) {
				const uint64 selectionStartByteOffset = min(component.inputText->cursorOffset, component.inputText->selectionMarkerOffset);
				const uint64 selectionEndByteOffset = max(component.inputText->cursorOffset, component.inputText->selectionMarkerOffset);

				if (previousByteOffset == selectionStartByteOffset) {
					selectionComponent.offset.x = x - component.padding.x;
				}

				if (byteOffset == selectionEndByteOffset) {
					selectionComponent.size.width.value = x - component.padding.x + width - selectionComponent.offset.x;
				}
			}
			else {
				if (previousByteOffset == component.inputText->cursorOffset) {
					selectionComponent.offset.x = characterOffsetX;
					selectionComponent.size.width.value = 2_px;
					selectionComponent.color = 0x5cafe7ff;
				}
			}

			characterOffsetX += glyph.advanceWidth * scale;
		}

		if (!component.inputText->hasSelection()) {
			if (byteOffset == component.inputText->cursorOffset) {
				selectionComponent.offset.x = characterOffsetX;
				selectionComponent.size.width.value = 2_px;
				selectionComponent.color = 0x5cafe7ff;
			}
		}
	}

	void GUILayer::updateTextInput(TextInputEvent& event, GUIContainer& component) {
		DEBUG_ASSERT(component.inputText != nullptr);

		component.inputText->insert(event.characterCode);
	}

	void GUILayer::updateTextInputAction(KeyDownEvent& event, GUIContainer& component) {
		UTF8String* string = component.inputText;

		DEBUG_ASSERT(string != nullptr);

		// TODO(gui/text-input/enter): Implement Enter key (execute command).
		// TODO(gui/text-input/tab): Implement Tab key for inline suggestions?
		// TODO(gui/text-input/escape): Implement Escape key (unfocus)?
		// TODO(gui/text-input/ctrl-backspace): Implement Ctrl + Backspace key combination (delete up to a character not in [A-Za-z0-9_])?
		switch (event.getKeyCode()) {
			case KeyCode::BACKSPACE: {
				if (string->getByteSize() == 0) {
					break;
				}

				string->select();
				string->moveCursor(-1);
				string->removeSelection();

				break;
			}
			case KeyCode::ARROW_LEFT: {
				if (string->cursorOffset > 0) {
					string->moveCursor(-1);
				}

				break;
			}
			case KeyCode::ARROW_UP: {
				string->seek(0);

				break;
			}
			case KeyCode::ARROW_RIGHT: {
				if (string->cursorOffset < string->getByteSize()) {
					string->moveCursor(1);
				}

				break;
			}
			case KeyCode::ARROW_DOWN: {
				string->seek(string->getByteSize());

				break;
			}
			case KeyCode::DELETE: {
				if (string->cursorOffset == string->getByteSize()) {
					break;
				}

				string->moveCursor(1);
				string->select();
				string->moveCursor(-1);
				string->removeSelection();

				break;
			}
			default: {
				break;
			}
		}
	}
}