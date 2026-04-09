// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/API/GUI/GUILayer.hpp"
#include "Engine/API/GUI/Text.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	void GUILayer::postConstruct() {
		renderer.setLayer(this);
		renderer.build();

		TextureArrayLayer defaultImageLayer;
		defaultImageLayer.extent = {1, 1};
		defaultImageLayer.data = {255, 255, 255, 255};

		renderer.registerTextureArrayLayer(defaultImageLayer);

		registerAssets();

		renderer.flushTextureArrayStagingBuffer();

		textureArrayFlushed = true;

		components = sequence<GUIContainer>(MAX_COMPONENT_COUNT);
		openComponentIndices = sequence<uint32>(MAX_COMPONENT_COUNT);
		leafComponentIndices = sequence<uint32>(MAX_COMPONENT_COUNT);
		buttonComponentIndices = sequence<uint32>(MAX_COMPONENT_COUNT);
	}

	FontID GUILayer::loadFontAsset(view<char8> assetName) {
		ENGINE_ASSERT(fontCount < MAX_FONT_COUNT, "Too many fonts loaded.");
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		const ref<FontAsset> fontAsset = getApplication()->getAssetLibrary()->getAsset<FontAsset>(assetName);
		const ref<ImageAsset> glyphMapImageAsset = fontAsset->getGlyphMapImage();

		fonts[fontCount].fontAsset = fontAsset;

		TextureArrayLayer glyphMapImageLayer{};
		glyphMapImageLayer.extent.width = glyphMapImageAsset->getExtent().width;
		glyphMapImageLayer.extent.height = glyphMapImageAsset->getExtent().height;
		glyphMapImageLayer.data = sequence<uint8>(glyphMapImageLayer.extent.width * glyphMapImageLayer.extent.height * getChannelCount(ImageFormat::R8G8B8A8_UNORM));

		if (glyphMapImageAsset->getFormat() == ImageFormat::R8_UNORM) {
			convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(glyphMapImageAsset->getData(), &glyphMapImageLayer.data[0], glyphMapImageLayer.data.count());
		}
		else if (glyphMapImageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
			convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(glyphMapImageAsset->getData(), &glyphMapImageLayer.data[0], glyphMapImageLayer.data.count());
		}
		else if (glyphMapImageAsset->getFormat() == ImageFormat::R8G8B8A8_UNORM) {
			std::copy(glyphMapImageAsset->getData().begin(), glyphMapImageAsset->getData().end(), &glyphMapImageLayer.data[0]);
		}
		else {
			ENGINE_ERROR_UNSUPPORTED();
		}

		const FontID fontId = fontCount;

		fonts[fontCount].glyphMapImageId = renderer.registerTextureArrayLayer(glyphMapImageLayer);
		fontCount += 1;

		return fontId;
	}

	ImageID GUILayer::loadImageAsset(view<char8> assetName) {
		ENGINE_ASSERT(!textureArrayFlushed, "Images can only be added when constructing the layer.");

		const ref<ImageAsset> imageAsset = getApplication()->getAssetLibrary()->getAsset<ImageAsset>(assetName);

		TextureArrayLayer imageLayer{};
		imageLayer.extent.width = imageAsset->getExtent().width;
		imageLayer.extent.height = imageAsset->getExtent().height;
		imageLayer.data = sequence<uint8>(imageLayer.extent.width * imageLayer.extent.height * getChannelCount(ImageFormat::R8G8B8A8_UNORM));

		if (imageAsset->getFormat() == ImageFormat::R8_UNORM) {
			convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageLayer.data[0], imageLayer.data.count());
		}
		else if (imageAsset->getFormat() == ImageFormat::R8G8B8_UNORM) {
			convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageLayer.data[0], imageLayer.data.count());
		}
		else if (imageAsset->getFormat() == ImageFormat::R8G8B8A8_UNORM) {
			std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), &imageLayer.data[0]);
		}
		else {
			ENGINE_ERROR_UNSUPPORTED();
		}

		return renderer.registerTextureArrayLayer(imageLayer);
	}

	void GUILayer::preRender() {
		componentCount = 0;
		openComponentIndexCount = 0;
		leafComponentIndexCount = 0;
		buttonComponentIndexCount = 0;

		openRootComponent();

		GUI();

		if (componentCount <= 1) {
			// No other components than root, return.
			return;
		}

		closeRootComponent();

		DispatchMouseEvents();

		uint32 componentIndex = 1;

		while (componentIndex < componentCount) {
			const GUIContainer& component = components[componentIndex];

			if (component.textComponent) {
				GUIGlyphSubcomponent defaultGlyphSubcomponent{};
				defaultGlyphSubcomponent.color = component.color.value;
				defaultGlyphSubcomponent.textureArrayLayerIndex = fonts[component.font].glyphMapImageId;
				defaultGlyphSubcomponent.fontSize = component.fontSize;

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
				registerSubcomponent(component);
			}

			componentIndex += 1;
		}
	}

	void GUILayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
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
		rootComponent.zIndex = 0;
		rootComponent.padding = {0_px, 0_px};
		rootComponent.anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;

		// TODO: Change to VERTICAL.
		rootComponent.layoutDirection = HORIZONTAL;

		rootComponent.layoutSpacing = 0_px;
		rootComponent.color = 0xffffffff;
		rootComponent.image = 0;
		rootComponent.pointerType = PointerType::DEFAULT;
		rootComponent.textComponent = false;
		rootComponent.debugName = "__root";

		components[componentCount] = rootComponent;
		componentCount += 1;

		openComponentIndices[openComponentIndexCount] = 0;
		openComponentIndexCount += 1;
	}

	void GUILayer::openComponent(const ContainerStyle& style) {
		const uint32 componentIndex = componentCount;
		const uint32 parentIndex = openComponentIndices[openComponentIndexCount - 1];

		GUIContainer& parent = components[parentIndex];

		GUIContainer component{};
		component.parentIndex = parentIndex;
		component.origin = parent.origin;
		component.size = style.size;
		component.zIndex = style.zIndex;

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
		component.pointerType = style.pointerType;
		component.textComponent = false;
		component.eventListeners = style.eventListeners;
		component.debugName = style.debugName;

		parent.childIndices.emplace_back(componentIndex);

		components[componentCount] = component;
		componentCount += 1;

		openComponentIndices[openComponentIndexCount] = componentIndex;
		openComponentIndexCount += 1;

		if (component.eventListeners != nullptr) {
			buttonComponentIndices[buttonComponentIndexCount] = componentIndex;
			buttonComponentIndexCount += 1;
		}
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

		for (sint32 componentIndex = static_cast<sint32>(componentCount) - 1; componentIndex >= 1; componentIndex -= 1) {
			FitSizingHeights(componentIndex);
		}

		GrowAndShrinkSizingHeightsRecursive(rootComponentIndex);
		PositionsAndAlignment(rootComponent, rootComponent.origin);

		openComponentIndexCount -= 1;
	}

	void GUILayer::closeComponent() {
		DEBUG_ASSERT(openComponentIndexCount > 0);

		const uint32 componentIndex = openComponentIndices[openComponentIndexCount - 1];

		FitSizingWidths(componentIndex);

		const GUIContainer& component = components[componentIndex];

		if (component.childIndices.size() == 0) {
			leafComponentIndices[leafComponentIndexCount] = componentIndex;
			leafComponentIndexCount += 1;
		}

		openComponentIndexCount -= 1;
	}

	std::vector<uint32> GUILayer::FindGrowableChildIndices(const GUIContainer& parent, uint8 axis) const {
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

	std::vector<uint32> GUILayer::FindShrinkableChildIndices(const GUIContainer& parent, uint8 axis) const {
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

	void GUILayer::FitSizingWidths(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];

		// Add padding to the component's size.
		component.size.width.value += component.padding.x * 2.0f;

		if (component.layoutDirection == HORIZONTAL) {
			// Add layout spacing to the component's size.
			const sint32 childCount = static_cast<sint32>(component.childIndices.size());
			const float32 layoutSpacing = component.layoutSpacing * static_cast<float32>(max(0, childCount - 1));

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
				parent.size.width.value = max(parent.size.width.value, component.size.width.value);
				parent.minimumSize.x = max(parent.minimumSize.x, component.minimumSize.x);
			}
		}

		parent.size.width.value = min(parent.size.width.value, parent.maximumSize.x);
		parent.maximumSize.x = max(parent.maximumSize.x, parent.minimumSize.x);
	}

	void GUILayer::FitSizingHeights(uint32 componentIndex) {
		GUIContainer& component = components[componentIndex];

		DEBUG_ASSERT(component.parentIndex != max<uint32>());

		GUIContainer& parent = components[component.parentIndex];

		float32 componentHeight = component.size.height.value + component.padding.y * 2.0f;

		if (component.layoutDirection == VERTICAL) {
			componentHeight += component.layoutSpacing * (max(0, static_cast<sint32>(component.childIndices.size()) - 1));
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
				parent.size.height.value = max(parent.size.height.value, componentHeight);
				parent.minimumSize.y = max(parent.minimumSize.y, component.minimumSize.y);
			}
		}

		parent.size.height.value = min(parent.size.height.value, parent.maximumSize.y);
		parent.maximumSize.y = max(parent.maximumSize.y, parent.minimumSize.y);

		component.size.height.value = componentHeight;
	}

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

			if (growableChildIndices.size() > 0) {
				std::vector<uint32> growableChildIndexCopy = growableChildIndices;

				// Grow children.
				while (remainingWidth > 0.0f) {
					if (growableChildIndexCopy.size() == 0) {
						break;
					}

					float32 smallestWidth = components[growableChildIndexCopy[0]].size.width.value;
					float32 secondSmallestWidth = infinity<float32>();
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
							secondSmallestWidth = min(secondSmallestWidth, width);
							widthToAdd = secondSmallestWidth - smallestWidth;
						}
					}

					// Double-check that the width to add is <= than the equal distribution of the width among children.
					widthToAdd = min(widthToAdd, remainingWidth / growableChildIndexCopy.size());

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

			if (shrinkableChildIndices.size() > 0) {
				std::vector<uint32> shrinkableChildIndexCopy = shrinkableChildIndices;

				// Shrink children.
				while (remainingWidth < 0.0f) {
					if (shrinkableChildIndexCopy.size() == 0) {
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
							secondLargestWidth = max(secondLargestWidth, width);
							widthToRemove = secondLargestWidth - largestWidth;
						}
					}

					// Double-check that the width to remove is >= than the equal distribution of the width among children.
					widthToRemove = max(widthToRemove, remainingWidth / shrinkableChildIndexCopy.size());

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

			if (growableChildIndices.size() > 0) {
				std::vector<uint32> growableChildIndexCopy = growableChildIndices;

				// Grow children.
				while (remainingHeight > 0.0f) {
					if (growableChildIndexCopy.size() == 0) {
						break;
					}

					float32 smallestHeight = components[growableChildIndexCopy[0]].size.height.value;
					float32 secondSmallestHeight = infinity<float32>();
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
							secondSmallestHeight = min(secondSmallestHeight, height);
							heightToAdd = secondSmallestHeight - smallestHeight;
						}
					}

					// Double-check that the height to add is <= than the equal distribution of the height among expanding children.
					heightToAdd = min(heightToAdd, remainingHeight / growableChildIndexCopy.size());

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

			if (shrinkableChildIndices.size() > 0) {
				std::vector<uint32> shrinkableChildIndexCopy = shrinkableChildIndices;

				// Shrink children.
				while (remainingHeight < 0.0f) {
					if (shrinkableChildIndexCopy.size() == 0) {
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
							secondLargestHeight = max(secondLargestHeight, height);
							heightToRemove = secondLargestHeight - largestHeight;
						}
					}

					// Double-check that the height to remove is >= than the equal distribution of the height among expanding children.
					heightToRemove = max(heightToRemove, remainingHeight / shrinkableChildIndexCopy.size());

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

	void GUILayer::PositionsAndAlignment(GUIContainer& component, svec2<float32> origin) {
		component.origin = origin;

		if (component.childIndices.size() == 0) {
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
			remainingAcross = max(remainingAcross, child.size[!component.layoutDirection].value);
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
		const sint32* mousePositionInteger = getApplication()->getWindow()->getMousePosition();
		const vec2<float32> mousePosition = {static_cast<float32>(mousePositionInteger[0]), static_cast<float32>(mousePositionInteger[1])};

		for (sint32 buttonComponentIndex = static_cast<sint32>(buttonComponentIndexCount) - 1; buttonComponentIndex >= 0; buttonComponentIndex -= 1) {
			const uint32 componentIndex = buttonComponentIndices[buttonComponentIndex];
			const GUIContainer& component = components[componentIndex];

			if (IsIntersecting(component, mousePosition)) {
				return componentIndex;
			}
		}

		return 0;
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
		const Window* window = getApplication()->getWindow();
		const sint32* mousePositionInteger = window->getMousePosition();
		const uint32 lastHoveredComponentIndex = GetHoveredComponentIndex();

		MouseMoveEvent mouseMoveEvent(mousePositionInteger[0], mousePositionInteger[1]);

		if (hoveredComponentIndex > 0 && hoveredComponentIndex < max<uint32>() && hoveredComponentIndex != lastHoveredComponentIndex) {
			// Dispatch mouse leave event.
			const GUIContainer& component = components[hoveredComponentIndex];
			component.eventListeners->onMouseLeave(mouseMoveEvent);
		}

		if (lastHoveredComponentIndex < max<uint32>() && lastHoveredComponentIndex != hoveredComponentIndex) {
			// Dispatch mouse hover (= mouse enter) event.
			const GUIContainer& component = components[lastHoveredComponentIndex];

			if (lastHoveredComponentIndex > 0) {
				component.eventListeners->onMouseHover(mouseMoveEvent);
			}

			window->updatePointerType(component.pointerType);
		}

		hoveredComponentIndex = lastHoveredComponentIndex;
	}

	bool GUILayer::onMouseDown(MouseDownEvent& event) {
		if (hoveredComponentIndex > 0 && hoveredComponentIndex < max<uint32>()) {
			// Dispatch mouse down event.
			const GUIContainer& component = components[hoveredComponentIndex];
			component.eventListeners->onMouseDown(event);
		}

		mouseDownComponentIndex = hoveredComponentIndex;

		return false;
	}

	bool GUILayer::onMouseUp(MouseUpEvent& event) {
		if (mouseDownComponentIndex > 0 && mouseDownComponentIndex < max<uint32>()) {
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

	void GUILayer::registerSubcomponent(const GUIContainer& component) {
		const vec2<float32> decimalOrigin = {component.origin[0], component.origin[1]};
		const vec2<float32> decimalSize = {component.size.width.value, component.size.height.value};
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(component.image);

		GUISubcomponent subcomponent{};
		subcomponent.worldTransform = mat3x3<float32>::scaling(decimalSize) * mat3x3<float32>::translation(decimalOrigin);
		subcomponent.textureTransform = mat3x3<float32>::scaling({
			static_cast<float32>(imageExtent.width) / renderer.TEXTURE_ARRAY_MAX_WIDTH,
			static_cast<float32>(imageExtent.height) / renderer.TEXTURE_ARRAY_MAX_HEIGHT,
		});
		subcomponent.color = component.color.value;
		subcomponent.zIndex = component.zIndex;
		subcomponent.textureArrayLayerIndex = component.image;

		renderer.registerSubcomponent(subcomponent);
	}

	// TODO(gui/immediate-hover)
	bool GUILayer::isCurrentComponentHovered() const {
		DEBUG_ASSERT(openComponentIndexCount > 0);

		const GUIContainer& component = components[openComponentIndexCount - 1];
		const Window* window = getApplication()->getWindow();
		const sint32* mousePositionInteger = window->getMousePosition();
		const vec2<float32> mousePosition = {static_cast<float32>(mousePositionInteger[0]), static_cast<float32>(mousePositionInteger[1])};

		const bool hovered = IsIntersecting(component, mousePosition);

		return hovered;
	}
}