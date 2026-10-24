// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Component/Container.hpp"
#include "Engine/API/GUI/GUILayer.hpp"
#include "Engine/API/GUI/Style/TextStyle.hpp"
#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Stream/UTF8Stream.hpp"

namespace Trident {
	float32 GUILayer::calculateTextPreferredWidth(UTF8Stream& stream, const FontAsset& fontAsset, float32 scale) {
		float32 preferredWidth = 0.0f;
		float32 lineWidth = 0.0f;
		bool firstLineGlyph = true;

		stream.seek(0);

		while (stream.getRemainingByteSize() > 0) {
			const uint32 codePoint = stream.read();

			if (codePoint == '\n') {
				preferredWidth = max(preferredWidth, lineWidth);
				lineWidth = 0.0f;
				firstLineGlyph = true;

				continue;
			}

			const Glyph& glyph = fontAsset[codePoint];

			if (firstLineGlyph) {
				firstLineGlyph = false;

				if (glyph.whitespace) {
					continue;
				}
			}

			lineWidth += glyph.advanceWidth * scale;
		}

		return max(preferredWidth, lineWidth);
	}

	float32 GUILayer::calculateTextMinimumWidth(UTF8Stream& stream, const FontAsset& fontAsset, float32 scale) {
		float32 minimumWordWidth = 0.0f;
		float32 currentWordWidth = 0.0f;

		stream.seek(0);

		while (stream.getRemainingByteSize() > 0) {
			const uint32 codePoint = stream.read();

			if (codePoint == '\n') {
				minimumWordWidth = max(minimumWordWidth, currentWordWidth);
				currentWordWidth = 0.0f;

				continue;
			}

			const Glyph& glyph = fontAsset[codePoint];

			if (glyph.whitespace) {
				minimumWordWidth = max(minimumWordWidth, currentWordWidth);
				currentWordWidth = 0.0f;

				continue;
			}

			currentWordWidth += glyph.advanceWidth * scale;
		}

		return max(minimumWordWidth, currentWordWidth);
	}

	void GUILayer::Text(view<char8> text, const TextStyle& style) {
		UTF8Stream stream(text);

		const uint32 componentIndex = componentCount;
		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];
		const FontAsset& fontAsset = *fonts[style.font].fontAsset;
		const uint32 dpi = getApplication()->getWindow()->getDpi();
		const float32 fontSizePt = fontAsset.pxToPt(style.fontSize, dpi);
		const float32 scale = fontAsset.calculateScale(fontSizePt, dpi);
		const float32 preferredWidth = calculateTextPreferredWidth(stream, fontAsset, scale);
		const float32 minimumWidth = calculateTextMinimumWidth(stream, fontAsset, scale);

		GUIContainer& parent = components[parentIndex];

		GUIContainer textComponent{};
		textComponent.parentIndex = parentIndex;
		textComponent.origin = parent.origin;
		textComponent.size.width.fillMode = FillMode::FIT;
		textComponent.size.width.value = preferredWidth;
		textComponent.size.height.fillMode = FillMode::FIT;
		textComponent.size.height.value = style.fontSize;
		textComponent.maximumSize = {preferredWidth, style.fontSize};
		textComponent.minimumSize = {minimumWidth, style.fontSize};
		textComponent.zIndex = parent.zIndex;
		textComponent.padding = {0_px, 0_px, 0_px, 0_px};
		textComponent.layoutDirection = HORIZONTAL;
		textComponent.layoutSpacing = 0_px;
		textComponent.color = style.color;
		textComponent.image = fonts[style.font].glyphMapImageId;
		textComponent.pointerType = PointerType::DEFAULT;
		textComponent.textComponent = true;
		textComponent.text = text;
		textComponent.font = style.font;
		textComponent.fontSize = style.fontSize;
		textComponent.debugName = style.debugName;

		// Pushing the component to the vector might invalidate the parent reference, so we update the parent's children indices before.
		parent.childIndices.emplace_back(componentIndex);

		components[componentCount] = textComponent;
		componentCount += 1;

		AdjustWidth(componentIndex);
	}

	void GUILayer::WrapText(GUIContainer& component) {
		const FontData& fontData = fonts[component.font];
		const FontAsset& fontAsset = *fontData.fontAsset;
		const uint32 dpi = getApplication()->getWindow()->getDpi();
		const float32 fontSizePt = fontAsset.pxToPt(component.fontSize, dpi);
		const float32 fontScale = fontAsset.calculateScale(fontSizePt, dpi);
		const float32 lineHeight = fontAsset.lineHeight * fontScale;

		vec2<float32> characterOffset = component.origin + vec2<float32>{component.padding.x, component.padding.y + fontAsset.ascent * fontScale};
		uint32 lineCount = 1;
		range<uint64, uint64> wordByteRange = {0, 0};
		float32 wordCharacterOffsetX = 0.0f;
		UTF8Stream stream(component.text);

		while (stream.getRemainingByteSize() > 0) {
			const uint64 previousByteOffset = stream.getByteOffset();

			uint32 codePoint = stream.read();

			const uint64 codePointByteCount = stream.getByteOffset() - previousByteOffset;

			if (codePoint == '\n') {
				FlushCurrentWord(stream, fontAsset, fontScale, characterOffset, component.characters, wordByteRange);

				characterOffset.x = 0.0f;
				characterOffset.y += lineHeight;
				wordCharacterOffsetX = 0.0f;
				lineCount += 1;

				continue;
			}

			const Glyph& glyph = fontAsset[codePoint];

			if (glyph.whitespace) {
				FlushCurrentWord(stream, fontAsset, fontScale, characterOffset, component.characters, wordByteRange);

				characterOffset.x += glyph.advanceWidth * fontScale;
				wordCharacterOffsetX = characterOffset.x;

				continue;
			}

			const float32 previousWordCharacterOffsetX = wordCharacterOffsetX + glyph.size.x * fontScale;

			wordCharacterOffsetX += glyph.advanceWidth * fontScale;

			if (previousWordCharacterOffsetX > component.size.width.value) {
				characterOffset.x = 0.0f;
				characterOffset.y += lineHeight;
				lineCount += 1;
				wordByteRange.count += stream.getByteOffset() - previousByteOffset;

				while (stream.getRemainingByteSize() > 0) {
					const uint64 previousByteOffset = stream.getByteOffset();

					codePoint = stream.read();

					const Glyph& glyph = fontAsset[codePoint];

					if (glyph.whitespace) {
						break;
					}

					wordByteRange.count += stream.getByteOffset() - previousByteOffset;
				}

				FlushCurrentWord(stream, fontAsset, fontScale, characterOffset, component.characters, wordByteRange);

				wordCharacterOffsetX = characterOffset.x;

				continue;
			}

			wordByteRange.count += codePointByteCount;
		}

		if (wordByteRange.count > 0 && wordByteRange.offset + wordByteRange.count == stream.getByteSize()) {
			UTF8Stream wordStream(view<char8>(&stream.getData()[wordByteRange.offset], wordByteRange.count));

			FlushWord(wordStream, fontAsset, fontScale, characterOffset, component.characters);
		}

		component.size.height.value = lineHeight * lineCount;
	}

	void GUILayer::FlushCurrentWord(const UTF8Stream& stream, const FontAsset& fontAsset, float32 fontScale, vec2<float32>& characterOffset, std::vector<TextCharacter>& characters, range<uint64, uint64>& wordByteRange) const {
		UTF8Stream wordStream(view<char8>(&stream.getData()[wordByteRange.offset], wordByteRange.count));

		FlushWord(wordStream, fontAsset, fontScale, characterOffset, characters);

		wordByteRange.offset = stream.getByteOffset();
		wordByteRange.count = 0;
	}

	void GUILayer::FlushWord(UTF8Stream& stream, const FontAsset& fontAsset, float32 fontScale, vec2<float32>& characterOffset, std::vector<TextCharacter>& characters) const {
		while (stream.getRemainingByteSize() > 0) {
			const uint32 codePoint = stream.read();
			const Glyph& glyph = fontAsset[codePoint];

			// TODO(gui/text/kerning)
			/* if (stream.getRemainingByteSize() > 0) {
				const uint32 nextCodePoint = stream.peek();
				const sint16 kerning = fontAsset.getKerning(codePoint, nextCodePoint);

				offset.x += kerning * fontScale;
			} */

			const TextCharacter character = createTextCharacter(glyph, characterOffset, fontScale);

			characters.emplace_back(character);

			characterOffset.x += glyph.advanceWidth * fontScale;
		}
	}

	void GUILayer::WrapTextChildrenRecursive(uint32 parentIndex) {
		GUIContainer& parentComponent = components[parentIndex];

		for (const uint32 childIndex : parentComponent.childIndices) {
			GUIContainer& childComponent = components[childIndex];

			if (!childComponent.textComponent && childComponent.type != GUIComponentType::TEXT_INPUT) {
				WrapTextChildrenRecursive(childIndex);

				continue;
			}

			if (childComponent.type == GUIComponentType::TEXT_INPUT) {
				WrapInputText(childComponent, childIndex);
			}
			else {
				WrapText(childComponent);
			}
		}
	}

	TextCharacter GUILayer::createTextCharacter(const Glyph& glyph, vec2<float32> offset, float32 fontScale) {
		offset.x += glyph.leftSideBearing * fontScale;
		offset.y -= glyph.yMax * fontScale;

		const vec2<float32> size = vec2<float32>(glyph.size) * fontScale;
		const vec2<float32> startOffset = offset;
		const vec2<float32> endOffset = startOffset + size;

		const vec2<float32> alignedStartOffset = floor(startOffset);
		const vec2<float32> alignedEndOffset = ceil(endOffset);
		const vec2<float32> alignedSize = alignedEndOffset - alignedStartOffset;

		const vec2<float32> startOffsetPercent = -(startOffset - alignedStartOffset) / alignedSize;
		const vec2<float32> endOffsetPercent = vec2<float32>{2.0f, 2.0f} - ((endOffset - alignedStartOffset) / alignedSize);

		TextCharacter character{};
		character.offset = alignedStartOffset;
		character.size = alignedSize;
		character.startTextureCoordinate = lerp(glyph.minCoordinate, glyph.maxCoordinate, startOffsetPercent);
		character.endTextureCoordinate = lerp(glyph.minCoordinate, glyph.maxCoordinate, endOffsetPercent);

		return character;
	}

	float32 GUILayer::calculateFontScale(ref<FontAsset> fontAsset, float32 fontSize) const {
		const uint32 dpi = getApplication()->getWindow()->getDpi();
		const float32 fontSizePt = fontAsset->pxToPt(fontSize, dpi);

		return fontAsset->calculateScale(fontSizePt, dpi);
	}
}