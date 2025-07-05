#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/Application/Application.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/GUI/GUILayer.hpp"

namespace ProjectA {
	void GUILayer::build() {
		renderer.build();

		TextureArrayLayer defaultImageLayer;
		defaultImageLayer.width = 1;
		defaultImageLayer.height = 1;
		defaultImageLayer.data = {255, 255, 255, 255};

		renderer.registerTextureArrayLayer(defaultImageLayer);

		registerAssets();
	}

	void GUILayer::postConstruct() {
		renderer.flushTextureArrayStagingBuffer();

		textureArrayFlushed = true;
	}

	FontID GUILayer::loadFont(std::string_view path) {
		ENGINE_ASSERT(fontCount < MAX_FONT_COUNT, "Too many fonts loaded.");
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		/* defaultFont->textureIndex = static_cast<uint32>(images.size());
		defaultFont->glyphMap.map = QUIVER_BITMAP_GLYPH_MAP;
		defaultFont->glyphMap.base = 32;
		defaultFont->glyphMap.count = 220;
		defaultFont->tileHeight = 12;
		defaultFont->tileSpacing = 1; */

		fonts[fontCount].font = createRef<TrueTypeFont>(path);

		const SDFGlyphMap& glyphMap = fonts[fontCount].font->getGlyphMap();

		TextureArrayLayer glyphMapImageLayer;
		glyphMapImageLayer.width = glyphMap.extent.width;
		glyphMapImageLayer.height = glyphMap.extent.height;
		glyphMapImageLayer.data = sequence<uint8>(glyphMap.data.count * 4);

		// Map R to RGBA.
		for (uint32 texelIndex = 0; texelIndex < glyphMap.data.count; texelIndex += 1) {
			glyphMapImageLayer.data[texelIndex * 4 + 0] = glyphMap.data[texelIndex];
			glyphMapImageLayer.data[texelIndex * 4 + 1] = 0;
			glyphMapImageLayer.data[texelIndex * 4 + 2] = 0;
			glyphMapImageLayer.data[texelIndex * 4 + 3] = 255;
		}

		const FontID fontId = fontCount;

		fonts[fontCount].glyphMapImageId = renderer.registerTextureArrayLayer(glyphMapImageLayer);

		fontCount += 1;

		return fontId;
	}

	ImageID GUILayer::loadImage(std::string_view path) {
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		const Stream stream = Filesystem::openStream(path);
		const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(stream);

		TextureArrayLayer imageLayer;
		imageLayer.width = file.extent.width;
		imageLayer.height = file.extent.height;
		imageLayer.data = file.data;

		return renderer.registerTextureArrayLayer(imageLayer);
	}

	void GUILayer::preRender() {
		components.clear();
		openComponents.clear();

		openRootComponent();
		GUI();
		closeRootComponent();

		if (components.size() == 1) {
			// No other components than root, return.
			return;
		}

		svec2<float32> origin = {0_px, 0_px};

		Positions(components[0], origin);

		// 7. Register components for rendering.
		for (uint32 componentIndex = 1; componentIndex < components.size(); componentIndex += 1) {
			const GUIContainer& component = components[componentIndex];

			if (component.textComponent) {
				GUIGlyphSubcomponent defaultGlyphSubcomponent;
				defaultGlyphSubcomponent.color = component.color.value;
				defaultGlyphSubcomponent.layerIndex = fonts[component.font].glyphMapImageId;

				const vec2<float32> origin = {component.origin.x, component.origin.y};

				for (const TextCharacter& textCharacter : component.characters) {
					GUIGlyphSubcomponent glyphSubcomponent = defaultGlyphSubcomponent;
					glyphSubcomponent.world =
						mat3x3<float32>::scaling(textCharacter.size) *
						mat3x3<float32>::translation(origin + textCharacter.offset);
					glyphSubcomponent.textureWorld = textCharacter.textureTransform;

					renderer.registerGlyphSubcomponent(glyphSubcomponent);
				}
			}
			else {
				// Validate radius.
				// TODO: lyah::min
				const float32 minHalfDimension = std::min(component.size.width.value, component.size.height.value) / 2.0f;

				// ENGINE_ASSERT(component.radius >= 0.0f && component.radius <= minHalfDimension, "Invalid radius.");

				registerSubcomponent(component.origin, component.size, component.image, component.color, component.radius);
			}
		}
	}

	void GUILayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	void GUILayer::postRender() {
		renderer.resetCounters();
	}

	void GUILayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	bool GUILayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return false;
	}

	void GUILayer::openRootComponent() {
		const Extent2D swapchainExtent = renderer.getSwapchain()->getExtent();
		// const Extent2D swapchainExtent = {0, 0};

		GUIContainer rootComponent{};
		rootComponent.parentIndex = ~0;
		rootComponent.origin = {0_px, 0_px};
		rootComponent.size.width.fillMode = FillMode::FIXED;
		rootComponent.size.width.value = static_cast<float32>(swapchainExtent.width);
		rootComponent.size.height.fillMode = FillMode::FIXED;
		rootComponent.size.height.value = static_cast<float32>(swapchainExtent.height);
		rootComponent.padding = {0_px, 0_px};
		rootComponent.layoutDirection = HORIZONTAL;
		rootComponent.layoutSpacing = 0_px;
		rootComponent.color = 0xffffffff;
		rootComponent.image = 0;
		rootComponent.radius = 0.0f;
		rootComponent.textComponent = false;

		rootComponent.debugName = "__root";

		components.emplace_back(rootComponent);
		openComponents.emplace_back(0);
	}

	void GUILayer::openComponent(const ContainerStyle& style) {
		const uint32 componentIndex = static_cast<uint32>(components.size());
		const uint32 parentIndex = openComponents.back();

		GUIContainer& parent = components[parentIndex];

		// TODO: Move these asserts after closing the component.
		// ENGINE_ASSERT(parent == nullptr || (parent->size.width.fillMode == FillMode::FIT && style.size.width.fillMode == FillMode::EXPAND), "An expanding component cannot be a child of a fit component in the same direction.");
		// ENGINE_ASSERT(parent == nullptr || (parent->size.height.fillMode == FillMode::FIT && style.size.height.fillMode == FillMode::EXPAND), "An expanding component cannot be a child of a fit component in the same direction.");

		GUIContainer component{};
		component.parentIndex = parentIndex;
		component.origin = parent.origin;
		component.size = style.size;
		component.padding = style.padding;
		component.layoutDirection = style.layoutDirection;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;
		component.radius = style.radius;
		component.textComponent = false;

		component.debugName = style.debugName;

		parent.childIndices.emplace_back(componentIndex);
		openComponents.emplace_back(componentIndex);
		components.emplace_back(component);
	}

	void GUILayer::GUIText(const std::string& string, const TextStyle& textStyle) {
		const uint32 componentIndex = static_cast<uint32>(components.size());
		const uint32 parentIndex = openComponents.back();
		const float32 preferredWidth = static_cast<float32>(calculateTextPreferredWidth(string, textStyle));
		const float32 minimumWidth = static_cast<float32>(calculateTextMinimumWidth(string, textStyle));

		GUIContainer& parent = components[parentIndex];

		GUIContainer textComponent{};
		textComponent.parentIndex = parentIndex;
		textComponent.origin = parent.origin;
		textComponent.size.width.fillMode = FillMode::FIT;
		textComponent.size.width.value = preferredWidth;
		textComponent.size.height.fillMode = FillMode::FIT;
		textComponent.size.height.value = 0_px;
		textComponent.padding = {0_px, 0_px};
		textComponent.layoutDirection = HORIZONTAL;
		textComponent.layoutSpacing = 0_px;
		textComponent.color = textStyle.color;
		textComponent.image = fonts[textStyle.font].glyphMapImageId;
		textComponent.radius = 0.0f;
		textComponent.textComponent = true;
		textComponent.text = string;
		textComponent.preferredSize = {preferredWidth, 0_px};
		textComponent.minimumSize = {minimumWidth, 0_px};
		textComponent.font = textStyle.font;
		textComponent.fontSize = textStyle.fontSize;

		textComponent.debugName = textStyle.debugName;

		// Pushing the component to the vector might invalidate the parent reference, so we update the parent's children indices before.
		parent.childIndices.emplace_back(componentIndex);
		components.emplace_back(textComponent);

		FitSizingWidths(componentIndex);
	}

	void GUILayer::closeRootComponent() {
		const uint32 componentIndex = openComponents.back();

		GrowAndShrinkSizingWidths(componentIndex);
		WrapTextChildren(componentIndex);

		openComponents.pop_back();
	}

	void GUILayer::closeComponent() {
		DEBUG_ASSERT(!openComponents.empty());

		const uint32 componentIndex = openComponents.back();

		FitSizingWidths(componentIndex);
		GrowAndShrinkSizingWidths(componentIndex);

		WrapTextChildren(componentIndex);

		FitSizingHeights(componentIndex);

		// TODO
		// GrowAndShrinkSizingHeights(componentIndex);

		openComponents.pop_back();
	}

	std::vector<uint32> GUILayer::FindGrowableOrShrinkableChildren(const GUIContainer& parent, bool axis) const {
		std::vector<uint32> childIndices;

		for (uint32 index = 0; index < parent.childIndices.size(); index += 1) {
			const uint32 childIndex = parent.childIndices[index];
			const GUIContainer& child = components[childIndex];

			if (child.size[axis].fillMode == FillMode::FIT || child.size[axis].fillMode == FillMode::EXPAND) {
				childIndices.emplace_back(childIndex);
			}
		}

		return childIndices;
	}

	// TODO: lyah::max
	void GUILayer::FitSizingWidths(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];
		GUIContainer& parent = components[component.parentIndex];

		// Add padding to the component's size.
		component.size.width.value += component.padding.x * 2.0f;

		// Update the component and its parent depending on the parent's layout direction.
		if (parent.layoutDirection == HORIZONTAL) {
			// AAAA BB CCC D

			// Calculate layout spacing.
			const sint32 childCount = static_cast<sint32>(component.childIndices.size());
			const float32 layoutSpacing = component.layoutSpacing * static_cast<float32>(std::max(0, childCount - 1));

			component.size.width.value += layoutSpacing;

			if (parent.size.width.fillMode != FillMode::FIXED) {
				parent.size.width.value += component.size.width.value;
				parent.minimumSize.x += component.minimumSize.x;
			}
		}
		else {
			// AAAA
			// BB
			// CCC
			// D

			if (parent.size.width.fillMode != FillMode::FIXED) {
				parent.size.width.value = std::max(parent.size.width.value, component.size.width.value);
				parent.minimumSize.x = std::max(parent.minimumSize.x, component.minimumSize.x);
			}
		}
	}

	// TODO: lyah::max
	void GUILayer::FitSizingHeights(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];
		GUIContainer& parent = components[component.parentIndex];

		// Add padding to the component's size.
		component.size.height.value += component.padding.y * 2.0f;

		// Update the component and its parent depending on the parent's layout direction.
		if (parent.layoutDirection == VERTICAL) {
			// AAAA
			// BB
			// CCC
			// D

			// Calculate layout spacing.
			const sint32 childCount = static_cast<sint32>(component.childIndices.size());
			const float32 layoutSpacing = component.layoutSpacing * static_cast<float32>(std::max(0, childCount - 1));

			component.size.height.value += layoutSpacing;

			if (parent.size.height.fillMode != FillMode::FIXED) {
				parent.size.height.value += component.size.height.value;
				parent.minimumSize.y += component.minimumSize.y;
			}
		}
		else {
			// A B C D
			// A   C D
			//     C

			if (parent.size.height.fillMode != FillMode::FIXED) {
				parent.size.height.value = std::max(parent.size.height.value, component.size.height.value);
				parent.minimumSize.y = std::max(parent.minimumSize.y, component.minimumSize.y);
			}
		}
	}

	// TODO: lyah::max
	// TODO: lyah::min
	void GUILayer::GrowAndShrinkSizingWidths(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		const std::vector<uint32> growableOrShrinkableChildren = FindGrowableOrShrinkableChildren(parent, WIDTH_AXIS);

		if (growableOrShrinkableChildren.empty()) {
			return;
		}

		float32 remainingWidth = parent.size.width.value - parent.padding.x * 2.0f;

		if (parent.layoutDirection == HORIZONTAL) {
			// Grow/shrink towards right
			// a b c d

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				remainingWidth -= child.size.width.value;
			}

			remainingWidth -= parent.layoutSpacing * (parent.childIndices.size() - 1);
		}
		else {
			// Grow/shrink towards right
			// a
			// b
			// c
			// d

			float32 maxChildWidth = 0.0f;

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				maxChildWidth = std::max(maxChildWidth, child.size.width.value);
			}

			remainingWidth -= maxChildWidth;
		}

		// Grow children.
		while (remainingWidth > 0.0f) {
			float32 smallestWidth = components[growableOrShrinkableChildren[0]].size.width.value;
			float32 secondSmallestWidth = lyah::infinity<float32>();
			float32 widthToAdd = remainingWidth;

			for (const uint32 childIndex : growableOrShrinkableChildren) {
				const GUIContainer& child = components[childIndex];
				const float32 width = child.size.width.value;

				// If we have found a child smaller than the smallest width registered, replace it with the child's width.
				if (width < smallestWidth) {
					secondSmallestWidth = smallestWidth;
					smallestWidth = width;
				}

				// This child not the smallest.
				if (width > smallestWidth) {
					secondSmallestWidth = std::min(secondSmallestWidth, width);
					widthToAdd = secondSmallestWidth - smallestWidth;
				}
			}

			// Double-check that the width to add is <= than the equal distribution of the width among expanding children.
			widthToAdd = std::min(widthToAdd, remainingWidth / growableOrShrinkableChildren.size());

			// Add the width to each expanding child.
			for (const uint32 childIndex : growableOrShrinkableChildren) {
				GUIContainer& child = components[childIndex];

				// Resize the child in order to reach the second smallest width, then repeat.
				if (child.size.width.value == smallestWidth) {
					child.size.width.value += widthToAdd;

					// TODO: Handle maximum width.

					remainingWidth -= widthToAdd;
				}
			}
		}

		// Shrink children.
		while (remainingWidth < 0.0f) {
			float32 largestWidth = components[growableOrShrinkableChildren[0]].size.width.value;
			float32 secondLargestWidth = 0.0f;
			float32 widthToRemove = remainingWidth;

			for (const uint32 childIndex : growableOrShrinkableChildren) {
				const GUIContainer& child = components[childIndex];
				const float32 width = child.size.width.value;

				// If we have found a child larger than the largest width registered, replace it with the child's width.
				if (width > largestWidth) {
					secondLargestWidth = largestWidth;
					largestWidth = width;
				}

				// This child is not the largest.
				if (width < largestWidth) {
					secondLargestWidth = std::max(secondLargestWidth, width);
					widthToRemove = secondLargestWidth - largestWidth;
				}
			}

			// Double-check that the width to remove is >= than the equal distribution of the width among expanding children.
			widthToRemove = std::max(widthToRemove, remainingWidth / growableOrShrinkableChildren.size());

			// Remove the width from each expanding child.
			for (const uint32 childIndex : growableOrShrinkableChildren) {
				GUIContainer& child = components[childIndex];

				// Resize the child in order to reach the second largest width, then repeat.
				if (child.size.width.value == largestWidth) {
					child.size.width.value += widthToRemove;

					// TODO: Handle minimum width.

					remainingWidth -= widthToRemove;
				}
			}
		}
	}

	// TODO: lyah::max
	// TODO: lyah::min
	void GUILayer::GrowAndShrinkSizingHeights(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		const std::vector<uint32> growableOrShrinkableChildren = FindGrowableOrShrinkableChildren(parent, HEIGHT_AXIS);

		if (growableOrShrinkableChildren.empty()) {
			return;
		}

		float32 remainingHeight = parent.size.height.value - parent.padding.y * 2.0f;

		if (parent.layoutDirection == VERTICAL) {
			// Grow/shrink towards bottom
			// a
			// b
			// c
			// d

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				remainingHeight -= child.size.height.value;
			}

			remainingHeight -= parent.layoutSpacing * (parent.childIndices.size() - 1);
		}
		else {
			// Grow/shrink towards bottom
			// a b c d

			float32 maxChildHeight = 0.0f;

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				maxChildHeight = std::max(maxChildHeight, child.size.height.value);
			}

			remainingHeight -= maxChildHeight;
		}

		// Grow children.
		while (remainingHeight > 0.0f) {
			float32 smallestHeight = components[growableOrShrinkableChildren[0]].size.height.value;
			float32 secondSmallestHeight = lyah::infinity<float32>();
			float32 heightToAdd = remainingHeight;

			for (const uint32 childIndex : growableOrShrinkableChildren) {
				const GUIContainer& child = components[childIndex];
				const float32 height = child.size.height.value;

				// If we have found a child smaller than the smallest height registered, replace it with the child's height.
				if (height < smallestHeight) {
					secondSmallestHeight = smallestHeight;
					smallestHeight = height;
				}

				// This child not the smallest.
				if (height > smallestHeight) {
					secondSmallestHeight = std::min(secondSmallestHeight, height);
					heightToAdd = secondSmallestHeight - smallestHeight;
				}
			}

			// Double-check that the height to add is <= than the equal distribution of the height among expanding children.
			heightToAdd = std::min(heightToAdd, remainingHeight / growableOrShrinkableChildren.size());

			// Add the height to each expanding child.
			for (const uint32 childIndex : growableOrShrinkableChildren) {
				GUIContainer& child = components[childIndex];

				// Resize the child in order to reach the second smallest height, then repeat.
				if (child.size.height.value == smallestHeight) {
					child.size.height.value += heightToAdd;

					// TODO: Handle maximum height.

					remainingHeight -= heightToAdd;
				}
			}
		}

		// Shrink children.
		while (remainingHeight < 0.0f) {
			float32 largestHeight = components[growableOrShrinkableChildren[0]].size.height.value;
			float32 secondLargestHeight = 0.0f;
			float32 heightToRemove = remainingHeight;

			for (const uint32 childIndex : growableOrShrinkableChildren) {
				const GUIContainer& child = components[childIndex];
				const float32 height = child.size.height.value;

				// If we have found a child larger than the largest height registered, replace it with the child's height.
				if (height > largestHeight) {
					secondLargestHeight = largestHeight;
					largestHeight = height;
				}

				// This child is not the largest.
				if (height < largestHeight) {
					secondLargestHeight = std::max(secondLargestHeight, height);
					heightToRemove = secondLargestHeight - largestHeight;
				}
			}

			// Double-check that the height to remove is >= than the equal distribution of the height among expanding children.
			heightToRemove = std::max(heightToRemove, remainingHeight / growableOrShrinkableChildren.size());

			// Remove the height from each expanding child.
			for (const uint32 childIndex : growableOrShrinkableChildren) {
				GUIContainer& child = components[childIndex];

				// Resize the child in order to reach the second largest height, then repeat.
				if (child.size.height.value == largestHeight) {
					child.size.height.value += heightToRemove;

					// TODO: Handle minimum height.

					remainingHeight -= heightToRemove;
				}
			}
		}
	}

	void GUILayer::WrapTextChildren(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		for (const uint32 childIndex : parent.childIndices) {
			GUIContainer& child = components[childIndex];

			if (!child.textComponent) {
				continue;
			}

			WrapText(child);
		}
	}

	// A new line must be added when:
	// 1. The current word overflows the text component's width.
	// 2. A line break character (\n) is encountered.
	// 
	// The current word must be flushed when:
	// 1. It's the last word.
	// 2. It overflows the text component's width. However, we need to look ahead for the next blank character to close the word.
	// 3. A invisible character (space, \t, etc.) is encountered.
	// 
	// TODO: lyah::ceil
	void GUILayer::WrapText(GUIContainer& textComponent) {
		const FontData& fontData = fonts[textComponent.font];
		const TrueTypeFont& font = *fontData.font;
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(fontData.glyphMapImageId);
		const vec2<float32> imageSize = {static_cast<float32>(imageExtent.width), static_cast<float32>(imageExtent.height)};
		const float32 lineHeight = textComponent.fontSize / font.getLineHeight();

		svec2<float32> characterOffset = textComponent.origin;

		uint32 characterIndex = 0;
		uint32 wordStartIndex = 0;
		uint32 wordLength = 0;
		float32 postWordAdvanceWidth = 0.0f;

		bool flushCurrentWord = false;

		GUIGlyphSubcomponent defaultGlyphSubcomponent;
		defaultGlyphSubcomponent.layerIndex = fontData.glyphMapImageId;
		defaultGlyphSubcomponent.color = textComponent.color.value;

		while (characterIndex < textComponent.text.size()) {
			const char8 character = textComponent.text[characterIndex];

			characterIndex += 1;

			if (character >= 0) {
				const uint16 codePoint = static_cast<uint16>(character);
				const Glyph& glyph = font[codePoint];

				if (glyph.invisible) {
					// Flush the current word if we encounter a blank character.
					flushCurrentWord = true;
					postWordAdvanceWidth = glyph.advanceWidth * lineHeight;
				}
				else {
					const float32 width = glyph.width * lineHeight;

					if (characterOffset.x + width > textComponent.size.width.value) {
						characterOffset.x = 0.0f;
						characterOffset.y += textComponent.fontSize;

						wordLength += 1;

						for (uint32 lookAheadIndex = characterIndex; lookAheadIndex < textComponent.text.size(); lookAheadIndex += 1) {
							const char8 character = textComponent.text[lookAheadIndex];

							if (OutsideWord(character, font)) {
								break;
							}

							wordLength += 1;
						}

						characterIndex += wordLength - 1;
						flushCurrentWord = true;
					}
				}
			}
			else {
				// TODO: Handle UTF-16 and UTF-32 characters.
				//       For now, we just flush the current word.
				flushCurrentWord = true;
			}

			if (flushCurrentWord) {
				FlushWordData flushWordData;
				flushWordData.word = textComponent.text.substr(wordStartIndex, wordLength);
				flushWordData.font = &font;
				flushWordData.imageSize = imageSize;
				flushWordData.fontSize = textComponent.fontSize;
				flushWordData.lineHeight = lineHeight;

				FlushWord(flushWordData, textComponent.characters, characterOffset);

				wordStartIndex = characterIndex;
				wordLength = 0;
				flushCurrentWord = false;

				if (postWordAdvanceWidth > 0.0f) {
					characterOffset.x += postWordAdvanceWidth;

					postWordAdvanceWidth = 0.0f;
				}

				continue;
			}

			wordLength += 1;
		}

		// Case A: Flush last word.
		if (wordLength > 0 && wordStartIndex + wordLength == textComponent.text.size()) {
			FlushWordData flushWordData;
			flushWordData.word = textComponent.text.substr(wordStartIndex, wordLength);
			flushWordData.font = &font;
			flushWordData.imageSize = imageSize;
			flushWordData.fontSize = textComponent.fontSize;
			flushWordData.lineHeight = lineHeight;

			FlushWord(flushWordData, textComponent.characters, characterOffset);
		}
	}

	bool GUILayer::OutsideWord(char8 character, const TrueTypeFont& font) const {
		if (character < 0) {
			// TODO: Handle UTF-16 and UTF-32 characters.
			return true;
		}

		const uint16 codePoint = static_cast<uint16>(character);
		const Glyph& glyph = font[codePoint];

		return glyph.invisible;
	}

	void GUILayer::FlushWord(const FlushWordData& data, std::vector<TextCharacter>& textCharacters, svec2<float32>& characterOffset) {
		for (const char8 character : data.word) {
			const uint16 codePoint = static_cast<uint16>(character);
			const Glyph& glyph = (*data.font)[codePoint];
			const float32 advanceWidth = std::ceil(glyph.advanceWidth * data.lineHeight);
			const vec2<float32> startTextureCoordinate = {glyph.minCoordinate.x, glyph.minCoordinate.y};
			const vec2<float32> endTextureCoordinate = {glyph.maxCoordinate.x, glyph.maxCoordinate.y};

			TextCharacter textCharacter;
			textCharacter.offset = {characterOffset.x + glyph.leftSideBearing * data.lineHeight, characterOffset.y};
			textCharacter.size = {glyph.width * data.lineHeight, data.fontSize};
			textCharacter.textureTransform =
				mat3x3<float32>::scaling((endTextureCoordinate - startTextureCoordinate) * data.imageSize / renderer.TEXTURE_ARRAY_MAX_SIZE) *
				mat3x3<float32>::translation(startTextureCoordinate);

			textCharacters.emplace_back(textCharacter);

			characterOffset.x += advanceWidth;
		}
	}

	void GUILayer::Positions(GUIContainer& parent, svec2<float32>& origin) {
		parent.origin = origin;

		if (parent.childIndices.empty()) {
			// Skip over the component towards the layout direction (this includes the padding).
			origin[parent.layoutDirection] += parent.size[parent.layoutDirection].value;

			return;
		}

		// Add top/left padding.
		origin.x += parent.padding.x;
		origin.y += parent.padding.y;

		for (uint32 index = 0; index < parent.childIndices.size(); index += 1) {
			GUIContainer& child = components[parent.childIndices[index]];

			Positions(child, origin);

			// Add layout spacing if it's not the last child.
			if (index != parent.childIndices.size()) {
				origin[parent.layoutDirection] += parent.layoutSpacing;
			}
		}

		// Add again padding towards the layout direction.
		origin[parent.layoutDirection] += parent.padding[parent.layoutDirection];

		// Remove some padding away from the layout direction.
		origin[parent.layoutDirection] -= parent.padding[parent.layoutDirection];
	}

	void GUILayer::registerSubcomponent(svec2<float32> origin, const Size2D& size, ImageID imageId, const Color& color, float32 radius) {
		const vec2<float32> decimalOrigin = {origin[0], origin[1]};
		const vec2<float32> decimalSize = {size.width.value, size.height.value};
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(imageId);

		GUISubcomponent subcomponent;
		subcomponent.minBounds = {decimalOrigin[0] + radius, decimalOrigin[1] + radius};
		subcomponent.maxBounds = {decimalOrigin[0] + decimalSize[0] - radius, decimalOrigin[1] + decimalSize[1] - radius};
		subcomponent.worldTransform = mat3x3<float32>::scaling(decimalSize) * mat3x3<float32>::translation(decimalOrigin);
		subcomponent.textureTransform = mat3x3<float32>::scaling({
			static_cast<float32>(imageExtent.width) / renderer.TEXTURE_ARRAY_MAX_WIDTH,
			static_cast<float32>(imageExtent.height) / renderer.TEXTURE_ARRAY_MAX_HEIGHT,
		});
		subcomponent.layerIndex = imageId;
		subcomponent.color = color.value;
		subcomponent.radius = radius;

		renderer.registerSubcomponent(subcomponent);
	}

	bool GUILayer::intersects(Extent2D origin, Extent2D size, Extent2D point) {
		return (
			point.width >= origin.width &&
			point.width < origin.width + size.width &&
			point.height >= origin.height &&
			point.height < origin.height + size.height
		);
	}

	uint32 GUILayer::calculateTextPreferredWidth(const std::string& string, const TextStyle& textStyle) const {
		const TrueTypeFont& font = *fonts[textStyle.font].font;

		uint32 preferredWidth = 0;
		uint32 lineWidth = 0;

		bool firstLineGlyph = true;

		for (const char8 character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				// TODO: lyah::max
				preferredWidth = std::max(preferredWidth, lineWidth);

				lineWidth = 0;
				firstLineGlyph = true;

				continue;
			}

			const uint16 codePoint = static_cast<uint16>(character);
			const Glyph& glyph = font[codePoint];

			if (firstLineGlyph) {
				firstLineGlyph = false;

				if (glyph.invisible) {
					continue;
				}
			}

			const float32 advanceWidth = std::ceil(glyph.advanceWidth / static_cast<float32>(font.getLineHeight()) * textStyle.fontSize);

			lineWidth += static_cast<uint32>(advanceWidth);
		}

		// TODO: lyah::max
		preferredWidth = std::max(preferredWidth, lineWidth);

		return preferredWidth;
	}

	// TODO: lyah::max
	uint32 GUILayer::calculateTextMinimumWidth(const std::string& string, const TextStyle& textStyle) const {
		const TrueTypeFont& font = *fonts[textStyle.font].font;
		const float32 lineHeight = textStyle.fontSize / font.getLineHeight();

		uint32 minimumWordWidth = 0;
		uint32 currentWordWidth = 0;
		const Glyph* glyph = nullptr;

		bool closeWord = false;

		for (const char8 character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				closeWord = true;
			}
			else {
				const uint16 codePoint = static_cast<uint16>(character);

				glyph = &font[codePoint];

				if (glyph->invisible) {
					closeWord = true;
				}
			}

			if (closeWord) {
				minimumWordWidth = std::max(minimumWordWidth, currentWordWidth);
				currentWordWidth = 0;

				continue;
			}

			const uint32 advanceWidth = static_cast<uint32>(glyph->advanceWidth / textStyle.fontSize);

			currentWordWidth += advanceWidth;
		}

		return std::max(minimumWordWidth, currentWordWidth);
	}
}