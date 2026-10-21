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

			// TODO(gui/component/text-input/selection): Selection color.
			.color = 0xff9800ff,
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

		while (stream.getRemainingByteSize() > 0) {
			const uint64 previousByteOffset = stream.getByteOffset();
			const uint32 codePoint = stream.read();
			const uint64 byteOffset = stream.getByteOffset();

			DEBUG_ASSERT(codePoint != '\n');

			const Glyph& glyph = fontAsset[codePoint];

			if (glyph.whitespace) {
				characterOffsetX += glyph.advanceWidth * scale;

				continue;
			}

			const float32 x = horizontalHinting + ceil(characterOffsetX + glyph.leftSideBearing * scale);
			const float32 y = baseline - glyph.yMax * scale;

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
					selectionComponent.offset.x = textCharacter.offset.x;
				}

				if (byteOffset == selectionEndByteOffset) {
					selectionComponent.size.width.value = textCharacter.offset.x + textCharacter.size.x - selectionComponent.offset.x;
				}
			}

			characterOffsetX += glyph.advanceWidth * scale;
		}
	}

	void GUILayer::updateTextInput(TextInputEvent& event, GUIContainer& component) {
		DEBUG_ASSERT(component.inputText != nullptr);

		component.inputText->insert(event.characterCode);
	}

	void GUILayer::updateTextInputAction(KeyDownEvent& event, GUIContainer& component) {
		DEBUG_ASSERT(component.inputText != nullptr);

		// TODO(gui/text-input/enter): Implement Enter key (execute command).
		// TODO(gui/text-input/tab): Implement Tab key for inline suggestions?
		// TODO(gui/text-input/escape): Implement Escape key (unfocus)?
		// TODO(gui/text-input/ctrl-backspace): Implement Ctrl + Backspace key combination (delete up to a blank character)?
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
			default: {
				break;
			}
		}
	}
}