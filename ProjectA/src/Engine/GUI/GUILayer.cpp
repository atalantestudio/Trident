#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/Application.hpp"
#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/Component/ScriptComponent.hpp"
#include "Engine/GUI/GUILayer.hpp"
#include "Engine/Renderer/Font/Bitmap/BitmapFont.hpp"
#include "Engine/Renderer/Font/Bitmap/BitmapGlyphMap.hpp"

namespace ProjectA {
	GUILayer::GUILayer() {
		defaultImageIndex = renderer.registerDefaultImage();

		loadDefaultFont();

		renderer.build();

		//ecs.registerComponent<GUIComponent>();
		//ecs.registerComponent<ScriptComponent>();
	}

	/*GUILayer::~GUILayer() {
		ComponentPool<ScriptComponent>& componentPool = ecs.getComponentPool<ScriptComponent>();

		ScriptComponent* begin = componentPool.components.data();

		for (uint32 componentIndex = 0; componentIndex < componentPool.componentCount; componentIndex += 1) {
			ScriptComponent* component = begin + componentIndex;

			component->entity->onDestroy();
			component->destroyScript(component);
		}
	}*/

	void GUILayer::loadDefaultFont() {
		defaultFont = createScope<TrueTypeFont>("../ProjectA/src/Asset/Font/test.ttf");
		defaultFontImageIndex = renderer.registerImage(defaultFont->getSdfImage());

		/*defaultFont->textureIndex = static_cast<uint32>(images.size());
		defaultFont->glyphMap.map = QUIVER_BITMAP_GLYPH_MAP;
        defaultFont->glyphMap.base = 32;
        defaultFont->glyphMap.count = 220;
        defaultFont->tileHeight = 12;
        defaultFont->tileSpacing = 1;

		fonts.emplace_back(defaultFont);*/
	}

	/*void GUILayer::registerEntityPool() {
		const ComponentPool<GUIComponent>& componentPool = ecs.getComponentPool<GUIComponent>();

		ENGINE_ASSERT(componentPool.componentCount < MAX_GUI_COMPONENTS, "Too many GUI components.");

		if (componentPool.componentCount == 0) {
			ENGINE_WARN("No GUI component registered.");

			return;
		}

		renderer.registerEntityPool(componentPool);
	}*/

	void GUILayer::preRender() {
		//updateScriptComponents();

		const ref<Swapchain>& swapchain = Renderer::getSwapchain();

		componentOrigin[HORIZONTAL] = 0;
		componentOrigin[VERTICAL] = 0;
		layoutDirection = VERTICAL;
		layoutSpacing = 0;
		currentContainerSize = {
			static_cast<sint16>(swapchain->getExtentWidth()),
			static_cast<sint16>(swapchain->getExtentHeight()),
		};
		currentContainerInnerSize = currentContainerSize;

		GUI();
	}

	void GUILayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	void GUILayer::postRender() {
		renderer.resetCounters();
	}

	void GUILayer::GUI() {}

	void GUILayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	bool GUILayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return true;
	}

	ImageID GUILayer::loadImage(std::string_view path) {
		return renderer.registerImage(path);
	}

	/*void GUILayer::updateScriptComponents() const {
		ComponentPool<ScriptComponent>& componentPool = ecs.getComponentPool<ScriptComponent>();

		ScriptComponent* begin = componentPool.components.data();

		for (uint32 componentIndex = 0; componentIndex < componentPool.componentCount; componentIndex += 1) {
			ScriptComponent* component = begin + componentIndex;

			if (component->entity == nullptr) {
				component->entity = component->constructScript();
				component->entity->onConstruct();
			}

			component->entity->onUpdate();
		}
	}*/

	GUIContainer GUILayer::pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing) {
		// TODO: Ensure that the container isn't expanding towards the current layout direction.

		GUIContainer container;
		container.origin = {componentOrigin[0], componentOrigin[1]};
		container.padding = padding;
		container.size = computeFixedSize(size);
		container.innerSize = container.size;
		container.innerSize.width -= padding.width * 2;
		container.innerSize.height -= padding.height * 2;
		container.parentSize = currentContainerSize;
		container.parentInnerSize = currentContainerInnerSize;
		container.root = rootComponent;
		container.parentLayoutDirection = this->layoutDirection;
		container.parentLayoutSpacing = this->layoutSpacing;

		// Add container padding to the origin.
		componentOrigin[HORIZONTAL] += padding.width;
		componentOrigin[VERTICAL] += padding.height;

		currentContainerSize = container.size;
		currentContainerInnerSize = container.innerSize;

		// Set layout direction.
		this->layoutDirection = layoutDirection;

		// Set layout spacing (dependent of the current layout direction).
		this->layoutSpacing = layoutSpacing;

		rootComponent = false;

		return container;
	}

	GUIContainer GUILayer::pushContainer(GUISize size, GUISize padding, LayoutDirection layoutDirection, uint16 layoutSpacing, const vec4<float32>& color) {
		const GUIContainer container = pushContainer(size, padding, layoutDirection, layoutSpacing);

		const GUISize originShift = {
			static_cast<sint16>(-padding.width),
			static_cast<sint16>(-padding.height),
		};

		registerQuadComponent(container.origin, container.size, defaultImageIndex, color);

		return container;
	}

	void GUILayer::registerQuadComponent(GUISize origin, GUISize size, ImageID imageId, const vec4<float32>& color) {
		const GUISize fixedSize = computeFixedSize(size);
		const vec2<float32> fixedSizeFloat = {
			static_cast<float32>(fixedSize[0]),
			static_cast<float32>(fixedSize[1]),
		};
		const vec2<float32> originFloat = {
			static_cast<float32>(origin[0]),
			static_cast<float32>(origin[1]),
		};
		const mat3x3<float32> transform = mat3x3<float32>::scaling(fixedSizeFloat) * mat3x3<float32>::translation(originFloat);
		const mat3x3<float32> imageTransform = mat3x3<float32>::identity();

		renderer.registerSubcomponent(transform, imageTransform, imageId, color);
	}

	void GUILayer::GUIQuad(GUISize size, const vec4<float32>& color) {
		return GUIQuad(size, defaultImageIndex, color);
	}

	void GUILayer::GUIQuad(GUISize size, ImageID imageId, const vec4<float32>& color) {
		registerQuadComponent({componentOrigin[0], componentOrigin[1]}, size, imageId, color);

		const GUISize fixedSize = computeFixedSize(size);

		componentOrigin[layoutDirection] += fixedSize[layoutDirection] + layoutSpacing;
	}

	void GUILayer::GUIText(const std::string& string, const TextStyle& textStyle) {
		const float32 lineHeight = static_cast<float32>(defaultFont->getLineHeight());

		float32 positionX = componentOrigin[0];
		float32 positionY = componentOrigin[1];

		float32 width = 0;
		const float32 height = textStyle.fontSize;

		for (const char glyph : string) {
			const uint16 codePoint = static_cast<uint16>(glyph);
			const TrueTypeGlyph& glyph = (*defaultFont)[codePoint];
			const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * textStyle.fontSize);

			if (!glyph.invisible) {
				// For a line height (ascent - descent) of 1210 FUnits,
				// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
				// The width should be 62.13px.
				// TODO: float32 lyah::ceil(float32)
				const float32 width = glyph.width / lineHeight * textStyle.fontSize;
				const float32 leftSideBearing = glyph.leftSideBearing / lineHeight * textStyle.fontSize;

				const vec2<float32> position = {positionX + leftSideBearing, positionY};
				const vec2<float32> size = {width, textStyle.fontSize};
				const mat3x3<float32> transform = mat3x3<float32>::scaling(size) * mat3x3<float32>::translation(position);

				const vec2<float32> uvStart = {glyph.minCoordinateX, glyph.minCoordinateY};
				const vec2<float32> uvEnd = {glyph.maxCoordinateX, glyph.maxCoordinateY};
				const mat3x3<float32> imageTransform = mat3x3<float32>::scaling(uvEnd - uvStart) * mat3x3<float32>::translation(uvStart);

				renderer.registerGlyphSubcomponent(transform, imageTransform, defaultFontImageIndex, textStyle);
			}

			positionX += advanceWidth;
			width += advanceWidth;
		}

		const GUISize fixedSize = {static_cast<sint16>(width), static_cast<sint16>(height)};

		componentOrigin[layoutDirection] += fixedSize[layoutDirection] + layoutSpacing;
	}

	bool GUILayer::GUIButton(const std::string& string, const ButtonStyle& buttonStyle) {
		// 1. Calculate the minimum size that fully encloses the text.
		GUISize size;

		size.width = 0;
		size.height = static_cast<sint16>(buttonStyle.textStyle->fontSize);

		for (const char glyph : string) {
			const uint16 codePoint = static_cast<uint16>(glyph);
			const TrueTypeGlyph& glyph = (*defaultFont)[codePoint];
			const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * buttonStyle.textStyle->fontSize);

			size.width += static_cast<sint16>(advanceWidth);
		}

		// 2. Register a quad having the same size.
		registerQuadComponent({componentOrigin[0], componentOrigin[1]}, size, defaultImageIndex, {0.5f, 0.5f, 0.5f, 1.0f});

		// 3. Register the text on top of it.
		{
			const float32 lineHeight = static_cast<float32>(defaultFont->getLineHeight());

			float32 positionX = componentOrigin[0];
			float32 positionY = componentOrigin[1];

			for (const char glyph : string) {
				const uint16 codePoint = static_cast<uint16>(glyph);
				const TrueTypeGlyph& glyph = (*defaultFont)[codePoint];
				const float32 advanceWidth = std::ceil(glyph.advanceWidth / 1210.0f * buttonStyle.textStyle->fontSize);

				if (!glyph.invisible) {
					// For a line height (ascent - descent) of 1210 FUnits,
					// the 'A' glyph's width (not including bearing or advance) is 631 FUnits.
					// The width should be 62.13px.
					// TODO: float32 lyah::ceil(float32)
					const float32 width = glyph.width / lineHeight * buttonStyle.textStyle->fontSize;
					const float32 leftSideBearing = glyph.leftSideBearing / lineHeight * buttonStyle.textStyle->fontSize;

					const vec2<float32> position = {positionX + leftSideBearing, positionY};
					const vec2<float32> size = {width, buttonStyle.textStyle->fontSize};
					const mat3x3<float32> transform = mat3x3<float32>::scaling(size) * mat3x3<float32>::translation(position);

					const vec2<float32> uvStart = {glyph.minCoordinateX, glyph.minCoordinateY};
					const vec2<float32> uvEnd = {glyph.maxCoordinateX, glyph.maxCoordinateY};
					const mat3x3<float32> imageTransform = mat3x3<float32>::scaling(uvEnd - uvStart) * mat3x3<float32>::translation(uvStart);

					renderer.registerGlyphSubcomponent(transform, imageTransform, defaultFontImageIndex, *buttonStyle.textStyle);
				}

				positionX += advanceWidth;
			}
		}

		bool hovered = false;

		if (intersects(
			{componentOrigin[0], componentOrigin[1]},
			size,
			{static_cast<sint16>(Application::getWindow().getMousePosition()[0]), static_cast<sint16>(Application::getWindow().getMousePosition()[1])}
		)) {
			hovered = true;
		}

		// 4. Advance the component origin by the size (towards the current layout direction).
		componentOrigin[layoutDirection] += size[layoutDirection] + layoutSpacing;

		return hovered;
	}

	void GUILayer::popContainer(GUIContainer& container) {
		container._iteration = true;

		layoutDirection = container.parentLayoutDirection;
		layoutSpacing = container.parentLayoutSpacing;

		if (container.root) {
			rootComponent = true;

			componentOrigin[0] = container.padding.width;
			componentOrigin[1] -= container.padding.height;
		}

		// Set the component origin to the end of the popped container.
		componentOrigin[layoutDirection] = container.origin[layoutDirection] + container.size[layoutDirection] + layoutSpacing;
		componentOrigin[!layoutDirection] = container.origin[!layoutDirection];

		// Set the current container sizes to the parent container sizes.
		currentContainerSize = container.parentSize;
		currentContainerInnerSize = container.parentInnerSize;
	}

	bool GUILayer::intersects(GUISize origin, GUISize size, GUISize point) {
		return (
			point.width >= origin.width &&
			point.width < origin.width + size.width &&
			point.height >= origin.height &&
			point.height < origin.height + size.height
		);
	}

	GUISize GUILayer::computeFixedSize(GUISize size) const {
		GUISize fixedSize{};

		if (size.width == EXPAND) {
			fixedSize.width = currentContainerInnerSize.width;
		}
		else {
			fixedSize.width = size.width;
		}

		ENGINE_ASSERT(fixedSize.width > 0, "The fixed width must be positive.");

		if (size.height == EXPAND) {
			fixedSize.height = currentContainerInnerSize.height;
		}
		else {
			fixedSize.height = size.height;
		}

		ENGINE_ASSERT(fixedSize.height > 0, "The fixed height must be positive.");

		return fixedSize;
	}
}