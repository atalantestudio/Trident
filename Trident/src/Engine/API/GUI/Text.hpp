// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::GUIText(const sequence<char8>& text, const TextStyle& textStyle) {
		UTF8Stream stream(text);

		const uint32 componentIndex = static_cast<uint32>(components.size());
		const uint32 parentIndex = openComponentIndices.back();
		const float32 preferredWidth = calculateTextPreferredWidth(stream, textStyle);
		const float32 minimumWidth = calculateTextMinimumWidth(stream, textStyle);

		GUIContainer& parent = components[parentIndex];

		GUIContainer textComponent{};
		textComponent.parentIndex = parentIndex;
		textComponent.origin = parent.origin;
		textComponent.size.width.fillMode = FillMode::FIT;
		textComponent.size.width.value = preferredWidth;
		textComponent.size.height.fillMode = FillMode::FIT;
		textComponent.size.height.value = textStyle.fontSize;
		textComponent.maximumSize = {preferredWidth, textStyle.fontSize};
		textComponent.minimumSize = {minimumWidth, textStyle.fontSize};
		textComponent.padding = {0_px, 0_px};
		textComponent.layoutDirection = HORIZONTAL;
		textComponent.layoutSpacing = 0_px;
		textComponent.color = textStyle.color;
		textComponent.image = fonts[textStyle.font].glyphMapImageId;
		textComponent.pointerType = PointerType::DEFAULT;
		textComponent.textComponent = true;
		textComponent.text = text;
		textComponent.font = textStyle.font;
		textComponent.fontSize = textStyle.fontSize;
		textComponent.eventListeners = textStyle.eventListeners;
		textComponent.debugName = textStyle.debugName;

		// Pushing the component to the vector might invalidate the parent reference, so we update the parent's children indices before.
		parent.childIndices.emplace_back(componentIndex);
		components.emplace_back(textComponent);
		leafComponentIndices.emplace_back(componentIndex);

		if (textComponent.eventListeners != nullptr) {
			buttonComponentIndices.emplace_back(componentIndex);
		}

		FitSizingWidths(componentIndex);
	}

	void GUILayer::WrapText(GUIContainer& textComponent) {
		const FontData& fontData = fonts[textComponent.font];
		const FontAsset& fontAsset = *fontData.fontAsset;
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(fontData.glyphMapImageId);
		const vec2<float32> imageSize = {static_cast<float32>(imageExtent.width), static_cast<float32>(imageExtent.height)};
		const float32 lineHeight = textComponent.fontSize / fontAsset.getLineHeight();

		svec2<float32> characterOffset = textComponent.origin;
		uint32 lineCount = 1;
		uint64 wordByteOffset = 0;
		uint64 wordByteLength = 0;
		float32 wordCharacterOffsetX = 0.0f;
		float32 postWordAdvanceWidth = 0.0f;
		float32 postWordAdvanceHeight = 0.0f;

		bool flushCurrentWord = false;
		bool flushBeforeNewLine = false;

		GUIGlyphSubcomponent defaultGlyphSubcomponent{};
		defaultGlyphSubcomponent.color = textComponent.color.value;
		defaultGlyphSubcomponent.textureArrayLayerIndex = fontData.glyphMapImageId;
		defaultGlyphSubcomponent.fontSize = textComponent.fontSize;

		uint32 codePoint;

		UTF8Stream stream(textComponent.text);

		while (stream.getRemainingByteSize() > 0) {
			const uint64 previousByteOffset = stream.getByteOffset();

			codePoint = stream.read();

			if (codePoint == '\n') {
				lineCount += 1;
				flushCurrentWord = true;
				flushBeforeNewLine = true;
			}
			else {
				const Glyph& glyph = fontAsset[codePoint];

				if (glyph.whitespace) {
					// Flush the current word if we encounter a blank character.
					flushCurrentWord = true;
					postWordAdvanceWidth = glyph.advanceWidth * lineHeight;
				}
				else {
					const float32 advanceWidth = ceil(glyph.advanceWidth * lineHeight);

					wordCharacterOffsetX += advanceWidth;

					if (wordCharacterOffsetX > textComponent.size.width.value) {
						characterOffset.x = 0.0f;
						characterOffset.y += textComponent.fontSize;
						lineCount += 1;
						wordByteLength += stream.getByteOffset() - previousByteOffset;
						flushCurrentWord = true;

						while (stream.getRemainingByteSize() > 0) {
							const uint64 previousByteOffset = stream.getByteOffset();

							codePoint = stream.read();

							const Glyph& glyph = fontAsset[codePoint];

							if (glyph.whitespace) {
								break;
							}

							wordByteLength += stream.getByteOffset() - previousByteOffset;
						}
					}
				}
			}

			if (flushCurrentWord) {
				FlushWordData flushWordData;
				flushWordData.stream = UTF8Stream(view<char8>(pointer_cast<const char8*>(&stream.getData()[wordByteOffset]), wordByteLength));
				flushWordData.fontAsset = &fontAsset;
				flushWordData.imageSize = imageSize;
				flushWordData.fontSize = textComponent.fontSize;
				flushWordData.lineHeight = lineHeight;

				FlushWord(flushWordData, textComponent.characters, characterOffset);

				wordByteOffset = stream.getByteOffset();
				wordByteLength = 0;
				flushCurrentWord = false;

				characterOffset.x += postWordAdvanceWidth;
				characterOffset.y += postWordAdvanceHeight;

				postWordAdvanceWidth = 0.0f;
				postWordAdvanceHeight = 0.0f;

				if (flushBeforeNewLine) {
					characterOffset.x = 0.0f;
					characterOffset.y += textComponent.fontSize;

					flushBeforeNewLine = false;
				}

				wordCharacterOffsetX = characterOffset.x;

				continue;
			}

			wordByteLength += stream.getByteOffset() - previousByteOffset;
		}

		if (wordByteLength > 0 && wordByteOffset + wordByteLength == stream.getByteSize()) {
			FlushWordData flushWordData;
			flushWordData.stream = UTF8Stream(view<char8>(pointer_cast<const char8*>(&stream.getData()[wordByteOffset]), wordByteLength));
			flushWordData.fontAsset = &fontAsset;
			flushWordData.imageSize = imageSize;
			flushWordData.fontSize = textComponent.fontSize;
			flushWordData.lineHeight = lineHeight;

			FlushWord(flushWordData, textComponent.characters, characterOffset);
		}

		textComponent.size.height.value = textComponent.fontSize * lineCount;
	}

	void GUILayer::FlushWord(FlushWordData& data, std::vector<TextCharacter>& textCharacters, svec2<float32>& characterOffset) {
		uint32 codePoint;

		const vec2<float32> imageScale = data.imageSize / renderer.TEXTURE_ARRAY_MAX_SIZE;

		while (data.stream.getRemainingByteSize() > 0) {
			codePoint = data.stream.read();

			const Glyph& glyph = (*data.fontAsset)[codePoint];
			const vec2<float32> startTextureCoordinate = {glyph.minCoordinate.x, glyph.minCoordinate.y};
			const vec2<float32> endTextureCoordinate = {glyph.maxCoordinate.x, glyph.maxCoordinate.y};

			TextCharacter textCharacter{};
			textCharacter.offset = {characterOffset.x + glyph.leftSideBearing * data.lineHeight, characterOffset.y + glyph.topSideBearing * data.lineHeight};
			textCharacter.size = {glyph.width * data.lineHeight, glyph.height * data.lineHeight};
			textCharacter.textureTransform =
				mat3x3<float32>::scaling((endTextureCoordinate - startTextureCoordinate) * imageScale) *
				mat3x3<float32>::translation(startTextureCoordinate * imageScale);

			textCharacters.emplace_back(textCharacter);

			characterOffset.x += glyph.advanceWidth * data.lineHeight;
		}
	}

	float32 GUILayer::calculateTextPreferredWidth(UTF8Stream& stream, const TextStyle& textStyle) const {
		const ref<FontAsset> fontAsset = fonts[textStyle.font].fontAsset;
		const float32 scale = 1.0f / fontAsset->getLineHeight() * textStyle.fontSize;

		float32 preferredWidth = 0.0f;
		float32 lineWidth = 0.0f;
		uint32 codePoint;
		bool firstLineGlyph = true;

		stream.seek(0);

		while (stream.getRemainingByteSize() > 0) {
			codePoint = stream.read();

			if (codePoint == '\n') {
				preferredWidth = max(preferredWidth, lineWidth);
				lineWidth = 0;
				firstLineGlyph = true;

				continue;
			}

			const Glyph& glyph = (*fontAsset)[codePoint];

			if (firstLineGlyph) {
				firstLineGlyph = false;

				if (glyph.whitespace) {
					continue;
				}
			}

			const float32 advanceWidth = ceil(glyph.advanceWidth * scale);

			lineWidth += advanceWidth;
		}

		return max(preferredWidth, lineWidth);
	}

	float32 GUILayer::calculateTextMinimumWidth(UTF8Stream& stream, const TextStyle& textStyle) const {
		const FontAsset& fontAsset = *fonts[textStyle.font].fontAsset;

		const Glyph* glyph = nullptr;
		float32 minimumWordWidth = 0.0f;
		float32 currentWordWidth = 0.0f;
		uint32 codePoint;
		bool closeWord = false;

		stream.seek(0);

		while (stream.getRemainingByteSize() > 0) {
			codePoint = stream.read();

			if (codePoint == '\n') {
				closeWord = true;
			}
			else {
				glyph = &fontAsset[codePoint];

				if (glyph->whitespace) {
					closeWord = true;
				}
			}

			if (closeWord) {
				minimumWordWidth = max(minimumWordWidth, currentWordWidth);
				currentWordWidth = 0.0f;

				continue;
			}

			const float32 advanceWidth = glyph->advanceWidth / textStyle.fontSize;

			currentWordWidth += advanceWidth;
		}

		return max(minimumWordWidth, currentWordWidth);
	}
}