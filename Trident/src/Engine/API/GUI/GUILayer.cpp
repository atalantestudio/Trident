// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/API/GUI/AdjustPositions.hpp"
#include "Engine/API/GUI/AdjustSizing.hpp"
#include "Engine/API/GUI/GUILayer.hpp"
#include "Engine/API/GUI/Text.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	bool GUILayer::IsIntersecting(const GUIContainer& component, vec2<float32> point) {
		return (
			point[0] >= component.origin.x &&
			point[0] < component.origin.x + component.size.width.value &&
			point[1] >= component.origin.y &&
			point[1] < component.origin.y + component.size.height.value
		);
	}

	void GUILayer::postConstruct() {
		renderer.setLayer(this);
		renderer.build();

		TextureArrayLayer defaultImageLayer;
		defaultImageLayer.extent = {1, 1};
		defaultImageLayer.data = {255, 255, 255, 255};

		renderer.registerTextureArrayLayer(defaultImageLayer);

		registerAssets();

		renderer.flushTextureArrayStagingBuffer();

		components = sequence<GUIContainer>(MAX_COMPONENT_COUNT);
		previousComponents = sequence<GUIContainer>(MAX_COMPONENT_COUNT);
		openContainerIndices = sequence<uint32>(MAX_COMPONENT_COUNT);
		focusableComponentIndexToComponentIndex = sequence<uint32>(MAX_COMPONENT_COUNT);
	}

	FontID GUILayer::loadFontAsset(view<char8> assetName) {
		ENGINE_ASSERT(fontCount < MAX_FONT_COUNT, "Too many fonts loaded.");
		ENGINE_ASSERT(!renderer.isTextureArrayFlushed(), "New fonts cannot be registered after flushing the GUI texture array.");

		const ref<FontAsset> fontAsset = getApplication()->getAssetLibrary()->getAsset<FontAsset>(assetName);
		const ref<ImageAsset> glyphMapImageAsset = fontAsset->getGlyphMapImage();

		fonts[fontCount].fontAsset = fontAsset;

		TextureArrayLayer glyphMapImageLayer{};
		glyphMapImageLayer.extent.width = glyphMapImageAsset->getExtent().width;
		glyphMapImageLayer.extent.height = glyphMapImageAsset->getExtent().height;
		glyphMapImageLayer.data = sequence<uint8>(glyphMapImageLayer.extent.width * glyphMapImageLayer.extent.height * getChannelCount(ImageFormat::R8G8B8A8_UNORM));

		ENGINE_ASSERT(glyphMapImageAsset->getFormat() == ImageFormat::R8_UNORM, "Glyph map images must have the R8_UNORM format.");

		convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(glyphMapImageAsset->getData(), &glyphMapImageLayer.data[0], glyphMapImageLayer.data.count());

		const FontID fontId = fontCount;

		fonts[fontCount].glyphMapImageId = renderer.registerTextureArrayLayer(glyphMapImageLayer);
		fontCount += 1;

		return fontId;
	}

	ImageID GUILayer::loadImageAsset(view<char8> assetName) {
		ENGINE_ASSERT(!renderer.isTextureArrayFlushed(), "New images cannot be registered after flushing the GUI texture array.");

		const ref<ImageAsset> imageAsset = getApplication()->getAssetLibrary()->getAsset<ImageAsset>(assetName);

		TextureArrayLayer imageLayer{};
		imageLayer.extent.width = imageAsset->getExtent().width;
		imageLayer.extent.height = imageAsset->getExtent().height;
		imageLayer.data = sequence<uint8>(imageLayer.extent.width * imageLayer.extent.height * getChannelCount(ImageFormat::R8G8B8A8_UNORM));

		switch (imageAsset->getFormat()) {
			case ImageFormat::R8_UNORM: {
				convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageLayer.data[0], imageLayer.data.count());

				break;
			}
			case ImageFormat::R8G8B8_UNORM: {
				convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageLayer.data[0], imageLayer.data.count());

				break;
			}
			case ImageFormat::R8G8B8A8_UNORM: {
				std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), &imageLayer.data[0]);

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		return renderer.registerTextureArrayLayer(imageLayer);
	}

	void GUILayer::preRender() {
		componentCount = 0;
		openContainerIndexCount = 0;
		focusableComponentCount = 0;

		const Extent2D extent = getApplication()->getWindow()->getExtent();

		GUIContainer rootComponent{};
		rootComponent.type = GUIComponentType::DEFAULT;
		rootComponent.parentIndex = max<uint32>();
		rootComponent.origin = {0_px, 0_px};
		rootComponent.size = {static_cast<float32>(extent.width), FIT};
		rootComponent.zIndex = 0;
		rootComponent.maximumSize = {max<float32>(), max<float32>()};
		rootComponent.minimumSize = {0_px, 0_px};
		rootComponent.padding = {0_px, 0_px, 0_px, 0_px};
		rootComponent.anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;
		rootComponent.layoutDirection = LayoutDirection::VERTICAL;
		rootComponent.layoutOffset = 0_px;
		rootComponent.layoutSpacing = 0_px;
		rootComponent.color = clearColor;
		rootComponent.image = 0;
		rootComponent.pointerType = PointerType::DEFAULT;
		rootComponent.textComponent = false;
		rootComponent.debugName = "__root";

		components[componentCount] = rootComponent;
		componentCount += 1;

		openContainerIndices[openContainerIndexCount] = 0;
		openContainerIndexCount += 1;

		render();

		// No other components than root, return.
		if (componentCount <= 1) {
			return;
		}

		expandAndFitComponent(0, 0);
		WrapTextChildrenRecursive(0);

		sint32 componentIndex = static_cast<sint32>(componentCount) - 1;

		while (componentIndex >= 1) {
			AdjustHeight(componentIndex);

			componentIndex -= 1;
		}

		expandAndFitComponent(0, 1);
		PositionsAndAlignment(components[0], components[0].origin, true);

		std::copy(&components[0], &components[componentCount], &previousComponents[0]);

		previousComponentCount = componentCount;
		previousFocusableComponentCount = focusableComponentCount;
	}

	void GUILayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		renderer.render(commandEncoder);
	}

	void GUILayer::postRender() {
		renderer.resetCounters();
	}

	void GUILayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);
		eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));
		eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
		eventDispatcher.dispatch<MouseUpEvent>(BIND(onMouseUp));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	void GUILayer::openContainer(const ContainerStyle& style, bool floating) {
		const uint32 componentIndex = componentCount;
		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];

		GUIContainer& parent = components[parentIndex];

		GUIContainer component{};
		component.type = GUIComponentType::DEFAULT;
		component.parentIndex = parentIndex;
		component.floating = floating;
		component.origin = parent.origin;
		component.offset = style.offset;
		component.size = style.size;
		component.zIndex = parent.zIndex + style.zIndex;

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
		component.layoutOffset = 0_px;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;

		if (style.focusable) {
			component.pointerType = PointerType::INTERACTIVE;
		}
		else if (style.pointerType == PointerType::INHERIT) {
			component.pointerType = parent.pointerType;
		}
		else {
			component.pointerType = style.pointerType;
		}

		component.textComponent = false;
		component.debugName = style.debugName;

		parent.childIndices.emplace_back(componentIndex);

		components[componentCount] = component;
		componentCount += 1;

		openContainerIndices[openContainerIndexCount] = componentIndex;
		openContainerIndexCount += 1;

		if (style.focusable) {
			focusableComponentIndexToComponentIndex[focusableComponentCount] = componentIndex;
			focusableComponentCount += 1;
		}

		if (previousComponentCount >= componentCount) {
			const GUIContainer& component = previousComponents[componentCount - 1];
			const Window* window = getApplication()->getWindow();
			const sint32* mousePositionInteger = window->getMousePosition();
			const vec2<float32> mousePosition = {static_cast<float32>(mousePositionInteger[0]), static_cast<float32>(mousePositionInteger[1])};

			if (IsIntersecting(component, mousePosition)) {
				window->updatePointerType(component.pointerType);
			}
		}
	}

	// TODO(gui): Reuse GUILayer::openContainer.
	void GUILayer::openVerticalList(const ContainerStyle& style) {
		ENGINE_ASSERT(style.size.height.fillMode != FillMode::FIT, "FillMode::FIT cannot be used on the height of a vertical list.");
		ENGINE_ASSERT(style.layoutDirection != LayoutDirection::HORIZONTAL, "Vertical lists must have a vertical layout direction.");

		const uint32 componentIndex = componentCount;
		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];

		GUIContainer& parent = components[parentIndex];

		GUIContainer component{};
		component.type = GUIComponentType::VERTICAL_LIST;
		component.parentIndex = parentIndex;
		component.floating = false;
		component.origin = parent.origin;
		component.offset = style.offset;
		component.size = style.size;
		component.zIndex = parent.zIndex + style.zIndex;

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
		component.layoutOffset = style.layoutOffset;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;

		if (style.pointerType == PointerType::INHERIT) {
			component.pointerType = parent.pointerType;
		}
		else {
			component.pointerType = style.pointerType;
		}

		component.textComponent = false;
		component.debugName = style.debugName;

		parent.childIndices.emplace_back(componentIndex);

		components[componentCount] = component;
		componentCount += 1;

		openContainerIndices[openContainerIndexCount] = componentIndex;
		openContainerIndexCount += 1;

		if (previousComponentCount >= componentCount) {
			const GUIContainer& component = previousComponents[componentCount - 1];
			const Window* window = getApplication()->getWindow();
			const sint32* mousePositionInteger = window->getMousePosition();
			const vec2<float32> mousePosition = {static_cast<float32>(mousePositionInteger[0]), static_cast<float32>(mousePositionInteger[1])};

			if (IsIntersecting(component, mousePosition)) {
				window->updatePointerType(component.pointerType);
			}
		}
	}

	void GUILayer::closeContainer() {
		DEBUG_ASSERT(openContainerIndexCount > 0);

		const uint32 componentIndex = openContainerIndices[openContainerIndexCount - 1];

		AdjustWidth(componentIndex);

		openContainerIndexCount -= 1;
	}

	bool GUILayer::onKeyDown(KeyDownEvent& event) {
		if (event.getKeyCode() == KeyCode::TAB) {
			focusedComponentIndex = (focusedComponentIndex + 1) % (focusableComponentCount + 1);

			return false;
		}

		if (event.getKeyCode() == KeyCode::ENTER) {
			if (focusedComponentIndex < previousFocusableComponentCount) {
				enterComponentIndex = focusableComponentIndexToComponentIndex[focusedComponentIndex];
			}

			return false;
		}

		return false;
	}

	bool GUILayer::onMouseDown(MouseDownEvent& event) {
		mouseUp = false;
		lastMouseDownPosition = getMousePosition();

		focusedComponentIndex = max<uint32>();

		return false;
	}

	bool GUILayer::onMouseUp(MouseUpEvent& event) {
		mouseUp = true;
		lastMouseUpPosition = getMousePosition();

		onMouseUpClient(event);

		return false;
	}

	bool GUILayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return false;
	}

	const GUIContainer* GUILayer::CurrentComponent() const {
		if (previousComponentCount < componentCount) {
			return nullptr;
		}

		return &previousComponents[componentCount - 1];
	}

	bool GUILayer::IsHovered() const {
		return IsHovered(componentCount - 1);
	}

	bool GUILayer::IsNextHovered() const {
		return IsHovered(componentCount);
	}

	bool GUILayer::IsMouseDown() const {
		return IsMouseDown(componentCount - 1);
	}

	bool GUILayer::IsNextMouseDown() const {
		return IsMouseDown(componentCount);
	}

	bool GUILayer::IsMouseUp() const {
		return IsMouseUp(componentCount - 1);
	}

	bool GUILayer::IsClicked() {
		if (previousComponentCount < componentCount || !mouseUp) {
			return false;
		}

		if (enterComponentIndex == componentCount - 1) {
			enterComponentIndex = max<uint32>();

			return true;
		}

		const GUIContainer& component = previousComponents[componentCount - 1];
		const bool intersecting = IsIntersecting(component, lastMouseDownPosition) && IsIntersecting(component, lastMouseUpPosition);

		if (intersecting) {
			lastMouseDownPosition = {-1.0f, -1.0f};
			lastMouseUpPosition = {-1.0f, -1.0f};
		}

		return intersecting;
	}

	bool GUILayer::IsFocused() const {
		return IsFocused(componentCount - 1);
	}

	bool GUILayer::IsNextFocused() const {
		return IsFocused(componentCount);
	}

	bool GUILayer::IsHovered(uint32 componentIndex) const {
		if (previousComponentCount < componentIndex + 1) {
			return false;
		}

		const GUIContainer& component = previousComponents[componentIndex];
		const vec2<float32> mousePosition = getMousePosition();

		return IsIntersecting(component, mousePosition);
	}

	bool GUILayer::IsMouseDown(uint32 componentIndex) const {
		if (previousComponentCount < componentIndex + 1) {
			return false;
		}

		const GUIContainer& component = previousComponents[componentIndex];

		return IsIntersecting(component, lastMouseDownPosition);
	}

	bool GUILayer::IsMouseUp(uint32 componentIndex) const {
		if (previousComponentCount < componentIndex + 1 || !mouseUp) {
			return false;
		}

		const GUIContainer& component = previousComponents[componentIndex];

		return IsIntersecting(component, lastMouseUpPosition);
	}

	bool GUILayer::IsFocused(uint32 componentIndex) const {
		if (focusedComponentIndex >= previousFocusableComponentCount) {
			return false;
		}

		const uint32 currentlyFocusedComponentIndex = focusableComponentIndexToComponentIndex[focusedComponentIndex];

		return currentlyFocusedComponentIndex == componentIndex;
	}

	vec2<float32> GUILayer::getMousePosition() const {
		const Window* window = getApplication()->getWindow();
		const sint32* mousePositionInteger = window->getMousePosition();

		return {static_cast<float32>(mousePositionInteger[0]), static_cast<float32>(mousePositionInteger[1])};
	}
}