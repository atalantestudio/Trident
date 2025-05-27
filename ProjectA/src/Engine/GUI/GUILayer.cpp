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

		componentOrigin[HORIZONTAL] = 0;
		componentOrigin[VERTICAL] = 0;
		layoutDirection = VERTICAL;
		layoutSpacing = 0;
		currentContainerSize = {
			static_cast<sint16>(Application::get().getWindow().getSize()[0]),
			static_cast<sint16>(Application::get().getWindow().getSize()[0]),
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

		GUIFloatingQuad(container.origin, container.size, defaultImageIndex, color);

		return container;
	}

	void GUILayer::GUIFloatingQuad(GUISize origin, GUISize size, ImageID imageId, const vec4<float32>& color) {
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
		GUIFloatingQuad({componentOrigin[0], componentOrigin[1]}, size, imageId, color);

		const GUISize fixedSize = computeFixedSize(size);

		componentOrigin[layoutDirection] += fixedSize[layoutDirection] + layoutSpacing;
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