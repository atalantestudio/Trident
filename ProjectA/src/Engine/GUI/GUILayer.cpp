#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/Application/Application.hpp"
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

	FontID GUILayer::loadFontAsset(const ref<FontAsset>& fontAsset) {
		ENGINE_ASSERT(fontCount < MAX_FONT_COUNT, "Too many fonts loaded.");
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		// TODO: Make bitmap fonts work seamlessly with True-Type fonts.
		/* defaultFont->textureIndex = static_cast<uint32>(images.size());
		defaultFont->glyphMap.map = QUIVER_BITMAP_GLYPH_MAP;
		defaultFont->glyphMap.utfOffset = 32;
		defaultFont->glyphMap.count = 220;
		defaultFont->tileHeight = 12;
		defaultFont->tileSpacing = 1; */

		fonts[fontCount].fontAsset = fontAsset;

		TextureArrayLayer glyphMapImageLayer;
		glyphMapImageLayer.width = fontAsset->getGlyphMapImage()->getExtent().width;
		glyphMapImageLayer.height = fontAsset->getGlyphMapImage()->getExtent().height;
		glyphMapImageLayer.data = fontAsset->getGlyphMapImage()->getData();

		const FontID fontId = fontCount;

		fonts[fontCount].glyphMapImageId = renderer.registerTextureArrayLayer(glyphMapImageLayer);

		fontCount += 1;

		return fontId;
	}

	ImageID GUILayer::loadImage(std::string_view path) {
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		const sequence<uint8> data = Filesystem::openBuffer(path);
		const File<FileFormat::PNG> file = Filesystem::decode<FileFormat::PNG>(data);

		TextureArrayLayer imageLayer;
		imageLayer.width = file.extent.width;
		imageLayer.height = file.extent.height;
		imageLayer.data = file.data;

		return renderer.registerTextureArrayLayer(imageLayer);
	}

	void GUILayer::preRender() {
		components.clear();
		openComponentIndices.clear();
		leafComponentIndices.clear();
		buttonComponentIndices.clear();

		openRootComponent();

		GUI();

		if (components.size() == 1) {
			// No other components than root, return.
			return;
		}

		closeRootComponent();

		DispatchMouseEvents();

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
				// Validate border radius.
				// TODO: lyah::min
				const float32 minHalfDimension = std::min(component.size.width.value, component.size.height.value) / 2.0f;

				// ENGINE_ASSERT(component.borderRadius >= 0.0f && component.borderRadius <= minHalfDimension, "Invalid border radius.");

				registerSubcomponent(component.origin, component.size, component.image, component.color, component.outlineSize, component.outlineColor, component.borderRadius);
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

		eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
		eventDispatcher.dispatch<MouseUpEvent>(BIND(onMouseUp));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	void GUILayer::openRootComponent() {
		GUIContainer rootComponent{};
		rootComponent.parentIndex = ~0;
		rootComponent.origin = {0_px, 0_px};
		rootComponent.size = {FIT, FIT};
		rootComponent.padding = {0_px, 0_px};
		rootComponent.anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;
		rootComponent.layoutDirection = HORIZONTAL;
		rootComponent.layoutSpacing = 0_px;
		rootComponent.color = 0xffffffff;
		rootComponent.image = 0;
		rootComponent.outlineSize = 0_px;
		rootComponent.outlineColor = 0x00000000;
		rootComponent.borderRadius = 0.0f;
		rootComponent.textComponent = false;
		rootComponent.debugName = "__root";

		components.emplace_back(rootComponent);
		openComponentIndices.emplace_back(0);
	}

	void GUILayer::openComponent(const ContainerStyle& style) {
		const uint32 componentIndex = static_cast<uint32>(components.size());
		const uint32 parentIndex = openComponentIndices.back();

		GUIContainer& parent = components[parentIndex];

		GUIContainer component{};
		component.parentIndex = parentIndex;
		component.origin = parent.origin;
		component.size = style.size;

		if (component.size.width.fillMode == FillMode::FIXED) {
			component.maximumSize.x = component.size.width.value;
			component.minimumSize.x = component.size.width.value;
		}
		else {
			component.maximumSize.x = style.maximumSize.x;

			if (style.minimumSize.x == max<float32>()) {
				component.minimumSize.x = component.size.width.value;
			}
			else {
				component.minimumSize.x = style.minimumSize.x;
				component.size.width.value = style.minimumSize.x;
			}
		}

		if (component.size.height.fillMode == FillMode::FIXED) {
			component.maximumSize.y = component.size.height.value;
			component.minimumSize.y = component.size.height.value;
		}
		else {
			component.maximumSize.y = style.maximumSize.y;

			if (style.minimumSize.y == max<float32>()) {
				component.minimumSize.y = component.size.height.value;
			}
			else {
				component.minimumSize.y = style.minimumSize.y;
				component.size.height.value = style.minimumSize.y;
			}
		}

		component.padding = style.padding;
		component.anchorPoint = style.anchorPoint;
		component.layoutDirection = style.layoutDirection;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;
		component.outlineSize = style.outlineSize;
		component.outlineColor = style.outlineColor;
		component.borderRadius = style.borderRadius;
		component.textComponent = false;
		component.eventListeners = style.eventListeners;
		component.debugName = style.debugName;

		parent.childIndices.emplace_back(componentIndex);
		components.emplace_back(component);
		openComponentIndices.emplace_back(componentIndex);

		if (component.eventListeners != nullptr) {
			buttonComponentIndices.emplace_back(componentIndex);
		}
	}

	void GUILayer::GUIText(const std::string& string, const TextStyle& textStyle) {
		const uint32 componentIndex = static_cast<uint32>(components.size());
		const uint32 parentIndex = openComponentIndices.back();
		const float32 preferredWidth = static_cast<float32>(calculateTextPreferredWidth(string, textStyle));
		const float32 minimumWidth = static_cast<float32>(calculateTextMinimumWidth(string, textStyle));

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
		textComponent.outlineSize = 0_px;
		textComponent.outlineColor = 0x00000000;
		textComponent.borderRadius = 0.0f;
		textComponent.textComponent = true;
		textComponent.text = string;
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

	void GUILayer::closeRootComponent() {
		static constexpr uint32 rootComponentIndex = 0;

		GUIContainer& rootComponent = components[rootComponentIndex];

		for (const uint32 childIndex : rootComponent.childIndices) {
			GUIContainer& child = components[childIndex];

			if (!child.textComponent) {
				GrowAndShrinkSizingWidthsRecursive(childIndex);
			}
		}

		WrapTextChildrenRecursive(rootComponentIndex);

		for (sint32 componentIndex = static_cast<sint32>(components.size()) - 1; componentIndex >= 1; componentIndex -= 1) {
			FitSizingHeights(componentIndex);
		}

		GrowAndShrinkSizingHeightsRecursive(rootComponentIndex);
		PositionsAndAlignment(rootComponent, rootComponent.origin);

		openComponentIndices.pop_back();
	}

	void GUILayer::closeComponent() {
		DEBUG_ASSERT(!openComponentIndices.empty());

		const uint32 componentIndex = openComponentIndices.back();

		FitSizingWidths(componentIndex);

		const GUIContainer& component = components[componentIndex];

		if (component.childIndices.empty()) {
			leafComponentIndices.emplace_back(componentIndex);
		}

		openComponentIndices.pop_back();
	}

	std::vector<uint32> GUILayer::FindGrowableChildIndices(const GUIContainer& parent, bool axis) const {
		std::vector<uint32> childIndices;

		for (uint32 index = 0; index < parent.childIndices.size(); index += 1) {
			const uint32 childIndex = parent.childIndices[index];
			const GUIContainer& child = components[childIndex];

			if (child.size[axis].fillMode == FillMode::EXPAND) {
				childIndices.emplace_back(childIndex);
			}
		}

		return childIndices;
	}

	std::vector<uint32> GUILayer::FindShrinkableChildIndices(const GUIContainer& parent, bool axis) const {
		std::vector<uint32> childIndices;

		for (uint32 index = 0; index < parent.childIndices.size(); index += 1) {
			const uint32 childIndex = parent.childIndices[index];
			const GUIContainer& child = components[childIndex];

			if (child.size[axis].fillMode == FillMode::FIT) {
				childIndices.emplace_back(childIndex);
			}
		}

		return childIndices;
	}

	// TODO: lyah::max
	// TODO: lyah::min
	void GUILayer::FitSizingWidths(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];

		// Add padding to the component's size.
		component.size.width.value += component.padding.x * 2.0f;

		if (component.layoutDirection == HORIZONTAL) {
			// Add layout spacing to the component's size.
			const sint32 childCount = static_cast<sint32>(component.childIndices.size());
			const float32 layoutSpacing = component.layoutSpacing * static_cast<float32>(std::max(0, childCount - 1));

			component.size.width.value += layoutSpacing;
		}

		GUIContainer& parent = components[component.parentIndex];

		// Update the component and its parent depending on the parent's layout direction.
		if (parent.layoutDirection == HORIZONTAL) {
			if (parent.size.width.fillMode != FillMode::FIXED) {
				parent.size.width.value += component.size.width.value;
				parent.minimumSize.x += component.minimumSize.x;
			}
		}
		else {
			if (parent.size.width.fillMode != FillMode::FIXED) {
				parent.size.width.value = std::max(parent.size.width.value, component.size.width.value);
				parent.minimumSize.x = std::max(parent.minimumSize.x, component.minimumSize.x);
			}
		}

		parent.size.width.value = std::min(parent.size.width.value, parent.maximumSize.x);
		parent.maximumSize.x = std::max(parent.maximumSize.x, parent.minimumSize.x);
	}

	// TODO: lyah::max
	// TODO: lyah::min
	void GUILayer::FitSizingHeights(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];

		DEBUG_ASSERT(component.parentIndex != ~0);

		GUIContainer& parent = components[component.parentIndex];

		float32 componentHeight = component.size.height.value + component.padding.y * 2.0f;

		if (component.layoutDirection == VERTICAL) {
			componentHeight += component.layoutSpacing * (std::max(0, static_cast<sint32>(component.childIndices.size()) - 1));
		}

		// Update the component and its parent depending on the parent's layout direction.
		if (parent.layoutDirection == VERTICAL) {
			if (parent.size.height.fillMode != FillMode::FIXED) {
				parent.size.height.value += componentHeight;
				parent.minimumSize.y += component.minimumSize.y;
			}
		}
		else {
			if (parent.size.height.fillMode != FillMode::FIXED) {
				parent.size.height.value = std::max(parent.size.height.value, componentHeight);
				parent.minimumSize.y = std::max(parent.minimumSize.y, component.minimumSize.y);
			}
		}

		parent.size.height.value = std::min(parent.size.height.value, parent.maximumSize.y);
		parent.maximumSize.y = std::max(parent.maximumSize.y, parent.minimumSize.y);

		component.size.height.value = componentHeight;
	}

	// TODO: lyah::max
	// TODO: lyah::min
	void GUILayer::GrowAndShrinkSizingWidthsRecursive(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		const std::vector<uint32> growableChildIndices = FindGrowableChildIndices(parent, WIDTH_AXIS);

		if (parent.layoutDirection == HORIZONTAL) {
			float32 remainingWidth = parent.size.width.value - parent.padding.x * 2.0f;

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				remainingWidth -= child.size.width.value;
			}

			remainingWidth -= parent.layoutSpacing * (parent.childIndices.size() - 1);

			if (!growableChildIndices.empty()) {
				std::vector<uint32> growableChildIndexCopy = growableChildIndices;

				// Grow children.
				while (remainingWidth > 0.0f) {
					if (growableChildIndexCopy.empty()) {
						break;
					}

					float32 smallestWidth = components[growableChildIndexCopy[0]].size.width.value;
					float32 secondSmallestWidth = lyah::infinity<float32>();
					float32 widthToAdd = remainingWidth;

					for (const uint32 childIndex : growableChildIndexCopy) {
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

					// Double-check that the width to add is <= than the equal distribution of the width among children.
					widthToAdd = std::min(widthToAdd, remainingWidth / growableChildIndexCopy.size());

					for (sint32 index = static_cast<sint32>(growableChildIndexCopy.size()) - 1; index >= 0; index -= 1) {
						const uint32 childIndex = growableChildIndexCopy[index];
						GUIContainer& child = components[childIndex];

						const float32 previousWidth = child.size.width.value;

						if (child.size.width.value == smallestWidth) {
							child.size.width.value += widthToAdd;

							if (child.size.width.value > child.maximumSize.x) {
								child.size.width.value = child.maximumSize.x;

								growableChildIndexCopy.erase(growableChildIndexCopy.begin() + index);
							}

							remainingWidth -= child.size.width.value - previousWidth;
						}
					}
				}
			}

			const std::vector<uint32> shrinkableChildIndices = FindShrinkableChildIndices(parent, WIDTH_AXIS);

			if (!shrinkableChildIndices.empty()) {
				std::vector<uint32> shrinkableChildIndexCopy = shrinkableChildIndices;

				// Shrink children.
				while (remainingWidth < 0.0f) {
					if (shrinkableChildIndexCopy.empty()) {
						break;
					}

					float32 largestWidth = components[shrinkableChildIndexCopy[0]].size.width.value;
					float32 secondLargestWidth = 0.0f;
					float32 widthToRemove = remainingWidth;

					for (const uint32 childIndex : shrinkableChildIndexCopy) {
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

					// Double-check that the width to remove is >= than the equal distribution of the width among children.
					widthToRemove = std::max(widthToRemove, remainingWidth / shrinkableChildIndexCopy.size());

					for (sint32 index = static_cast<sint32>(shrinkableChildIndexCopy.size()) - 1; index >= 0; index -= 1) {
						const uint32 childIndex = shrinkableChildIndexCopy[index];
						GUIContainer& child = components[childIndex];

						const float32 previousWidth = child.size.width.value;

						if (child.size.width.value == largestWidth) {
							child.size.width.value += widthToRemove;

							if (child.size.width.value < child.minimumSize.x) {
								child.size.width.value = child.minimumSize.x;

								shrinkableChildIndexCopy.erase(shrinkableChildIndexCopy.begin() + index);
							}

							remainingWidth -= child.size.width.value - previousWidth;
						}
					}
				}
			}
		}
		else {
			for (const uint32 childIndex : growableChildIndices) {
				GUIContainer& child = components[childIndex];

				child.size.width.value = parent.size.width.value - parent.padding.x * 2.0f;
			}
		}

		for (const uint32 childIndex : parent.childIndices) {
			const GUIContainer& child = components[childIndex];

			if (child.textComponent) {
				continue;
			}

			GrowAndShrinkSizingWidthsRecursive(childIndex);
		}
	}

	// TODO: lyah::max
	// TODO: lyah::min
	void GUILayer::GrowAndShrinkSizingHeightsRecursive(uint32 parentIndex) {
		GUIContainer& parent = components[parentIndex];

		const std::vector<uint32> growableChildIndices = FindGrowableChildIndices(parent, HEIGHT_AXIS);

		if (parent.layoutDirection == VERTICAL) {
			float32 remainingHeight = parent.size.height.value - parent.padding.y * 2.0f;

			for (const uint32 childIndex : parent.childIndices) {
				const GUIContainer& child = components[childIndex];

				remainingHeight -= child.size.height.value;
			}

			remainingHeight -= parent.layoutSpacing * (parent.childIndices.size() - 1);

			if (!growableChildIndices.empty()) {
				std::vector<uint32> growableChildIndexCopy = growableChildIndices;

				// Grow children.
				while (remainingHeight > 0.0f) {
					if (growableChildIndexCopy.empty()) {
						break;
					}

					float32 smallestHeight = components[growableChildIndexCopy[0]].size.height.value;
					float32 secondSmallestHeight = lyah::infinity<float32>();
					float32 heightToAdd = remainingHeight;

					for (const uint32 childIndex : growableChildIndexCopy) {
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
					heightToAdd = std::min(heightToAdd, remainingHeight / growableChildIndexCopy.size());

					for (sint32 index = static_cast<sint32>(growableChildIndexCopy.size()) - 1; index >= 0; index -= 1) {
						const uint32 childIndex = growableChildIndexCopy[index];
						GUIContainer& child = components[childIndex];

						const float32 previousHeight = child.size.height.value;

						if (child.size.height.value == smallestHeight) {
							child.size.height.value += heightToAdd;

							if (child.size.height.value > child.maximumSize.y) {
								child.size.height.value = child.maximumSize.y;

								growableChildIndexCopy.erase(growableChildIndexCopy.begin() + index);
							}

							remainingHeight -= child.size.height.value - previousHeight;
						}
					}
				}
			}

			const std::vector<uint32> shrinkableChildIndices = FindShrinkableChildIndices(parent, WIDTH_AXIS);

			if (!shrinkableChildIndices.empty()) {
				std::vector<uint32> shrinkableChildIndexCopy = shrinkableChildIndices;

				// Shrink children.
				while (remainingHeight < 0.0f) {
					if (shrinkableChildIndexCopy.empty()) {
						break;
					}

					float32 largestHeight = components[shrinkableChildIndexCopy[0]].size.height.value;
					float32 secondLargestHeight = 0.0f;
					float32 heightToRemove = remainingHeight;

					for (const uint32 childIndex : shrinkableChildIndexCopy) {
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
					heightToRemove = std::max(heightToRemove, remainingHeight / shrinkableChildIndexCopy.size());

					for (sint32 index = static_cast<sint32>(shrinkableChildIndexCopy.size()) - 1; index >= 0; index -= 1) {
						const uint32 childIndex = shrinkableChildIndexCopy[index];
						GUIContainer& child = components[childIndex];

						const float32 previousHeight = child.size.height.value;

						if (child.size.height.value == largestHeight) {
							child.size.height.value += heightToRemove;

							if (child.size.height.value < child.minimumSize.x) {
								child.size.height.value = child.minimumSize.x;

								shrinkableChildIndexCopy.erase(shrinkableChildIndexCopy.begin() + index);
							}

							remainingHeight -= child.size.height.value - previousHeight;
						}
					}
				}
			}
		}
		else {
			for (const uint32 childIndex : growableChildIndices) {
				GUIContainer& child = components[childIndex];

				child.size.height.value = parent.size.height.value - parent.padding.y * 2.0f;
			}
		}

		for (const uint32 childIndex : parent.childIndices) {
			const GUIContainer& child = components[childIndex];

			if (child.textComponent) {
				continue;
			}

			GrowAndShrinkSizingHeightsRecursive(childIndex);
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

	// TODO: lyah::ceil
	void GUILayer::WrapText(GUIContainer& textComponent) {
		const FontData& fontData = fonts[textComponent.font];
		const FontAsset& fontAsset = *fontData.fontAsset;
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(fontData.glyphMapImageId);
		const vec2<float32> imageSize = {static_cast<float32>(imageExtent.width), static_cast<float32>(imageExtent.height)};
		const float32 lineHeight = textComponent.fontSize / fontAsset.getLineHeight();

		svec2<float32> characterOffset = textComponent.origin;
		uint32 lineCount = 1;
		uint32 characterIndex = 0;
		uint32 wordStartIndex = 0;
		uint32 wordLength = 0;
		float32 wordCharacterOffsetX = 0.0f;
		float32 postWordAdvanceWidth = 0.0f;
		float32 postWordAdvanceHeight = 0.0f;

		bool flushCurrentWord = false;
		bool flushBeforeNewLine = false;

		GUIGlyphSubcomponent defaultGlyphSubcomponent;
		defaultGlyphSubcomponent.layerIndex = fontData.glyphMapImageId;
		defaultGlyphSubcomponent.color = textComponent.color.value;

		while (characterIndex < textComponent.text.size()) {
			const char8 character = textComponent.text[characterIndex];

			characterIndex += 1;

			if (character >= 0) {
				const uint16 codePoint = static_cast<uint16>(character);

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
						const float32 advanceWidth = std::ceil(glyph.advanceWidth * lineHeight);

						wordCharacterOffsetX += advanceWidth;

						if (wordCharacterOffsetX > textComponent.size.width.value) {
							characterOffset.x = 0.0f;
							characterOffset.y += textComponent.fontSize;
							lineCount += 1;
							wordLength += 1;
							flushCurrentWord = true;

							for (uint32 lookAheadIndex = characterIndex; lookAheadIndex < textComponent.text.size(); lookAheadIndex += 1) {
								const char8 character = textComponent.text[lookAheadIndex];

								if (OutsideWord(character, fontAsset)) {
									break;
								}

								wordLength += 1;
								characterIndex += 1;
							}
						}
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
				flushWordData.word = std::string_view(&textComponent.text[wordStartIndex], wordLength);
				flushWordData.fontAsset = &fontAsset;
				flushWordData.imageSize = imageSize;
				flushWordData.fontSize = textComponent.fontSize;
				flushWordData.lineHeight = lineHeight;

				FlushWord(flushWordData, textComponent.characters, characterOffset);

				wordStartIndex = characterIndex;
				wordLength = 0;
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

			wordLength += 1;
		}

		if (wordLength > 0 && wordStartIndex + wordLength == textComponent.text.size()) {
			FlushWordData flushWordData;
			flushWordData.word = std::string_view(&textComponent.text[wordStartIndex], wordLength);
			flushWordData.fontAsset = &fontAsset;
			flushWordData.imageSize = imageSize;
			flushWordData.fontSize = textComponent.fontSize;
			flushWordData.lineHeight = lineHeight;

			FlushWord(flushWordData, textComponent.characters, characterOffset);
		}

		textComponent.size.height.value = textComponent.fontSize * lineCount;
	}

	bool GUILayer::OutsideWord(char8 character, const FontAsset& fontAsset) const {
		if (character < 0) {
			// TODO: Handle UTF-16 and UTF-32 characters.
			return true;
		}

		const uint16 codePoint = static_cast<uint16>(character);
		const Glyph& glyph = fontAsset[codePoint];

		return glyph.whitespace;
	}

	void GUILayer::FlushWord(const FlushWordData& data, std::vector<TextCharacter>& textCharacters, svec2<float32>& characterOffset) {
		for (const char8 character : data.word) {
			const uint16 codePoint = static_cast<uint16>(character);
			const Glyph& glyph = (*data.fontAsset)[codePoint];
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

	// TODO: lyah::max
	void GUILayer::PositionsAndAlignment(GUIContainer& component, svec2<float32> origin) {
		component.origin = origin;

		if (component.childIndices.empty()) {
			const GUIContainer& parent = components[component.parentIndex];

			// Skip over the component towards the layout direction (this includes the padding).
			origin[parent.layoutDirection] += component.size[parent.layoutDirection].value;

			return;
		}

		// Add top/left padding.
		origin.x += component.padding.x;
		origin.y += component.padding.y;

		svec2<float32> remainingSize = {component.size.width.value - component.padding.x * 2.0f, component.size.height.value - component.padding.y * 2.0f};
		float32 remainingAcross = 0.0f;

		for (const uint32 childIndex : component.childIndices) {
			const GUIContainer& child = components[childIndex];

			remainingSize[component.layoutDirection] -= child.size[component.layoutDirection].value;
			remainingAcross = std::max(remainingAcross, child.size[!component.layoutDirection].value);
		}

		remainingSize[component.layoutDirection] -= component.layoutSpacing * (static_cast<uint32>(component.childIndices.size()) - 1);
		remainingSize[!component.layoutDirection] -= remainingAcross;

		// Snap to anchor point along layout direction.
		const uint8 shiftAmountAlong = 4 - component.layoutDirection * 3;
		const uint8 maskAlong = 0b111000 >> component.layoutDirection * 3;
		const uint8 anchorRangeAlong = (static_cast<uint8>(component.anchorPoint) & maskAlong) >> shiftAmountAlong;
		const float32 fAlong = static_cast<float32>(anchorRangeAlong) * 0.5f;

		origin[component.layoutDirection] += remainingSize[component.layoutDirection] * fAlong;

		// Snap to anchor point across layout direction.
		const uint8 shiftAmountAcross = 1 + component.layoutDirection * 3;
		const uint8 maskAcross = 0b111000 >> (!component.layoutDirection) * 3;
		const uint8 anchorRangeAcross = (static_cast<uint8>(component.anchorPoint) & maskAcross) >> shiftAmountAcross;
		const float32 fAcross = static_cast<float32>(anchorRangeAcross) * 0.5f;
		const float32 componentAcross = component.size[!component.layoutDirection].value - component.padding[!component.layoutDirection] * 2.0f;
		const float32 originAcross = origin[!component.layoutDirection];

		for (uint32 index = 0; index < component.childIndices.size(); index += 1) {
			GUIContainer& child = components[component.childIndices[index]];

			const float32 remainingAcross = componentAcross - child.size[!component.layoutDirection].value;

			origin[!component.layoutDirection] = originAcross + remainingAcross * fAcross;

			PositionsAndAlignment(child, origin);

			origin[component.layoutDirection] += child.size[component.layoutDirection].value;

			if (index + 1 == component.childIndices.size()) {
				break;
			}

			// Add layout spacing (only if it's not the last child).
			origin[component.layoutDirection] += component.layoutSpacing;
		}

		// Add padding along the layout direction.
		origin[component.layoutDirection] += component.padding[component.layoutDirection];

		// Remove padding across the layout direction.
		origin[!component.layoutDirection] -= component.padding[!component.layoutDirection];
	}

	uint32 GUILayer::GetHoveredComponentIndex() const {
		const sint32* mousePositionInteger = Application::getWindow().getMousePosition();
		const vec2<float32> mousePosition = {static_cast<float32>(mousePositionInteger[0]), static_cast<float32>(mousePositionInteger[1])};

		for (sint32 buttonComponentIndex = static_cast<sint32>(buttonComponentIndices.size()) - 1; buttonComponentIndex >= 0; buttonComponentIndex -= 1) {
			const uint32 componentIndex = buttonComponentIndices[buttonComponentIndex];
			const GUIContainer& component = components[componentIndex];

			if (IsIntersecting(component, mousePosition)) {
				return componentIndex;
			}
		}

		return max<uint32>();
	}

	bool GUILayer::IsIntersecting(const GUIContainer& component, const vec2<float32>& point) {
		return (
			point[0] >= component.origin.x &&
			point[0] < component.origin.x + component.size.width.value &&
			point[1] >= component.origin.y &&
			point[1] < component.origin.y + component.size.height.value
		);
	}

	void GUILayer::DispatchMouseEvents() {
		const sint32* mousePositionInteger = Application::getWindow().getMousePosition();
		const uint32 lastHoveredComponentIndex = GetHoveredComponentIndex();

		MouseMoveEvent mouseMoveEvent(mousePositionInteger[0], mousePositionInteger[1]);

		if (hoveredComponentIndex != max<uint32>() && hoveredComponentIndex != lastHoveredComponentIndex) {
			// Dispatch mouse leave event.
			const GUIContainer& component = components[hoveredComponentIndex];
			component.eventListeners->onMouseLeave(mouseMoveEvent);
		}

		if (lastHoveredComponentIndex != max<uint32>() && lastHoveredComponentIndex != hoveredComponentIndex) {
			// Dispatch mouse hover (= mouse enter) event.
			const GUIContainer& component = components[lastHoveredComponentIndex];
			component.eventListeners->onMouseHover(mouseMoveEvent);
		}

		hoveredComponentIndex = lastHoveredComponentIndex;
	}

	bool GUILayer::onMouseDown(MouseDownEvent& event) {
		if (hoveredComponentIndex != max<uint32>()) {
			// Dispatch mouse down event.
			const GUIContainer& component = components[hoveredComponentIndex];
			component.eventListeners->onMouseDown(event);
		}

		mouseDownComponentIndex = hoveredComponentIndex;

		return false;
	}

	bool GUILayer::onMouseUp(MouseUpEvent& event) {
		if (mouseDownComponentIndex != max<uint32>()) {
			// Dispatch a mouse down event.
			const GUIContainer& component = components[mouseDownComponentIndex];
			component.eventListeners->onMouseUp(event);

			if (mouseDownComponentIndex == hoveredComponentIndex) {
				// Dispatch a mouse click event (mouse down, then up on the same component).
				component.eventListeners->onMouseClick(event);
			}
		}

		mouseDownComponentIndex = max<uint32>();

		return false;
	}

	bool GUILayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return false;
	}

	void GUILayer::registerSubcomponent(svec2<float32> origin, const Size2D& size, ImageID imageId, const Color& color, float32 outlineSize, const Color& outlineColor, float32 borderRadius) {
		const vec2<float32> decimalOrigin = {origin[0], origin[1]};
		const vec2<float32> decimalSize = {size.width.value, size.height.value};
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(imageId);

		GUISubcomponent subcomponent;
		subcomponent.minBounds = {decimalOrigin[0], decimalOrigin[1]};
		subcomponent.maxBounds = {decimalOrigin[0] + decimalSize[0], decimalOrigin[1] + decimalSize[1]};
		subcomponent.worldTransform = mat3x3<float32>::scaling(decimalSize) * mat3x3<float32>::translation(decimalOrigin);
		subcomponent.textureTransform = mat3x3<float32>::scaling({
			static_cast<float32>(imageExtent.width) / renderer.TEXTURE_ARRAY_MAX_WIDTH,
			static_cast<float32>(imageExtent.height) / renderer.TEXTURE_ARRAY_MAX_HEIGHT,
		});
		subcomponent.layerIndex = imageId;
		subcomponent.color = color.value;
		subcomponent.outlineSize = outlineSize;
		subcomponent.outlineColor = outlineColor.value;
		subcomponent.borderRadius = borderRadius;

		renderer.registerSubcomponent(subcomponent);
	}

	// TODO: lyah::max
	uint32 GUILayer::calculateTextPreferredWidth(const std::string& string, const TextStyle& textStyle) const {
		const FontAsset& fontAsset = *fonts[textStyle.font].fontAsset;

		uint32 preferredWidth = 0;
		uint32 lineWidth = 0;

		bool firstLineGlyph = true;

		for (const char8 character : string) {
			if (character < 0) {
				// TODO: Handle UTF-16 characters.

				continue;
			}

			if (character == '\n') {
				preferredWidth = std::max(preferredWidth, lineWidth);
				lineWidth = 0;
				firstLineGlyph = true;

				continue;
			}

			const uint16 codePoint = static_cast<uint16>(character);
			const Glyph& glyph = fontAsset[codePoint];

			if (firstLineGlyph) {
				firstLineGlyph = false;

				if (glyph.whitespace) {
					continue;
				}
			}

			const float32 advanceWidth = std::ceil(glyph.advanceWidth / static_cast<float32>(fontAsset.getLineHeight()) * textStyle.fontSize);

			lineWidth += static_cast<uint32>(advanceWidth);
		}

		return std::max(preferredWidth, lineWidth);
	}

	// TODO: lyah::max
	uint32 GUILayer::calculateTextMinimumWidth(const std::string& string, const TextStyle& textStyle) const {
		const FontAsset& fontAsset = *fonts[textStyle.font].fontAsset;
		const float32 lineHeight = textStyle.fontSize / fontAsset.getLineHeight();

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

				glyph = &fontAsset[codePoint];

				if (glyph->whitespace) {
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