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
		textComponent.padding = {0_px, 0_px};
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
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(fontData.glyphMapImageId);
		const vec2<float32> imageSize = {static_cast<float32>(imageExtent.width), static_cast<float32>(imageExtent.height)};
		const uint32 dpi = getApplication()->getWindow()->getDpi();
		const float32 fontSizePt = fontAsset.pxToPt(component.fontSize, dpi);
		const float32 scale = fontAsset.calculateScale(fontSizePt, dpi);
		const float32 lineHeight = fontAsset.getLineHeight() * scale;

		FlushWordData flushWordData{};
		flushWordData.fontAsset = &fontAsset;
		flushWordData.imageSize = imageSize;
		flushWordData.scale = scale;

		svec2<float32> characterOffset = component.origin;
		uint32 lineCount = 1;
		range<uint64, uint64> wordByteRange = {0, 0};
		float32 wordCharacterOffsetX = 0.0f;
		UTF8Stream stream(component.text);

		while (stream.getRemainingByteSize() > 0) {
			const uint64 previousByteOffset = stream.getByteOffset();

			uint32 codePoint = stream.read();

			const uint64 codePointByteCount = stream.getByteOffset() - previousByteOffset;

			if (codePoint == '\n') {
				FlushCurrentWord(flushWordData, component, stream, characterOffset, wordByteRange);

				characterOffset.x = 0.0f;
				characterOffset.y += lineHeight;
				wordCharacterOffsetX = 0.0f;
				lineCount += 1;

				continue;
			}

			const Glyph& glyph = fontAsset[codePoint];

			if (glyph.whitespace) {
				FlushCurrentWord(flushWordData, component, stream, characterOffset, wordByteRange);

				characterOffset.x += glyph.advanceWidth * scale;
				wordCharacterOffsetX = characterOffset.x;

				continue;
			}

			const float32 previousWordCharacterOffsetX = wordCharacterOffsetX + glyph.width * scale;

			wordCharacterOffsetX += glyph.advanceWidth * scale;

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

				FlushCurrentWord(flushWordData, component, stream, characterOffset, wordByteRange);

				wordCharacterOffsetX = characterOffset.x;

				continue;
			}

			wordByteRange.count += codePointByteCount;
		}

		if (wordByteRange.count > 0 && wordByteRange.offset + wordByteRange.count == stream.getByteSize()) {
			FlushWordData flushWordData{};
			flushWordData.stream = UTF8Stream(view<char8>(&stream.getData()[wordByteRange.offset], wordByteRange.count));
			flushWordData.fontAsset = &fontAsset;
			flushWordData.imageSize = imageSize;
			flushWordData.scale = scale;

			FlushWord(flushWordData, component.characters, characterOffset);
		}

		component.size.height.value = lineHeight * lineCount;
	}

	void GUILayer::FlushCurrentWord(
		FlushWordData& flushWordData,
		GUIContainer& component,
		const UTF8Stream& stream,
		svec2<float32>& characterOffset,
		range<uint64, uint64>& wordByteRange
	) {
		flushWordData.stream = UTF8Stream(view<char8>(&stream.getData()[wordByteRange.offset], wordByteRange.count));

		FlushWord(flushWordData, component.characters, characterOffset);

		wordByteRange.offset = stream.getByteOffset();
		wordByteRange.count = 0;
	}

	inline static float32 fract(float32 x) {
		return x - static_cast<sint32>(x);
	}

	void GUILayer::FlushWord(FlushWordData& data, std::vector<TextCharacter>& textCharacters, svec2<float32>& characterOffset) {
		constexpr float32 hintingY = -0.4f;

		const vec2<float32> imageScale = data.imageSize / GUIRenderer::TEXTURE_ARRAY_MAX_SIZE;

		const float32 baseline = characterOffset.y + data.fontAsset->getAscent() * data.scale + hintingY;

		while (data.stream.getRemainingByteSize() > 0) {
			const uint32 codePoint = data.stream.read();
			const Glyph& glyph = (*data.fontAsset)[codePoint];

			float32 startU = glyph.minCoordinate.x;
			float32 startV = glyph.minCoordinate.y;
			float32 endU = glyph.maxCoordinate.x;
			float32 endV = glyph.maxCoordinate.y;

			float32 x = characterOffset.x + glyph.leftSideBearing * data.scale;
			float32 y = baseline - (glyph.yMax + 1) * data.scale;

			// TODO(gui/text/kerning)
			/* if (data.stream.getRemainingByteSize() > 0) {
				const uint32 nextCodePoint = data.stream.peek();
				const sint16 kerning = data.fontAsset->getKerning(codePoint, nextCodePoint);

				x += kerning * data.scale;
			} */

			float32 width = glyph.width * data.scale;
			float32 height = glyph.height * data.scale;

			x = round(x);

			TextCharacter textCharacter{};
			textCharacter.offset = {x, y};
			textCharacter.size = {width, height};
			textCharacter.startTextureCoordinate = vec2<float32>{startU, startV} * imageScale;
			textCharacter.endTextureCoordinate = vec2<float32>{endU, endV} * imageScale;

			textCharacters.emplace_back(textCharacter);

			characterOffset.x += glyph.advanceWidth * data.scale;
		}
	}

	void GUILayer::WrapTextChildrenRecursive(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		for (const uint32 childIndex : parent.childIndices) {
			GUIContainer& child = components[childIndex];

			if (!child.textComponent) {
				WrapTextChildrenRecursive(childIndex);

				continue;
			}

			WrapText(child);
		}
	}
}