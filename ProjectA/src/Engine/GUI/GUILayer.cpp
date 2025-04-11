#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/Component/ScriptComponent.hpp"
#include "Engine/GUI/GUILayer.hpp"
#include "Engine/Renderer/Font/Bitmap/BitmapFont.hpp"
#include "Engine/Renderer/Font/Bitmap/BitmapGlyphMap.hpp"

namespace ProjectA {
	GUILayer::GUILayer() {
		//ecs.registerComponent<GUIComponent>();
		//ecs.registerComponent<ScriptComponent>();
	}

	GUILayer::~GUILayer() {
		/*ComponentPool<ScriptComponent>& componentPool = ecs.getComponentPool<ScriptComponent>();

		ScriptComponent* begin = componentPool.components.data();

		for (uint32 componentIndex = 0; componentIndex < componentPool.componentCount; componentIndex += 1) {
			ScriptComponent* component = begin + componentIndex;

			component->entity->onDestroy();
			component->destroyScript(component);
		}*/
	}

	void GUILayer::loadDefaultFont() {
		const ref<BitmapFont> defaultFont = createRef<BitmapFont>();
		defaultFont->textureIndex = static_cast<uint32>(images.size());
		defaultFont->glyphMap.map = QUIVER_BITMAP_GLYPH_MAP;
        defaultFont->glyphMap.base = 32;
        defaultFont->glyphMap.count = 220;
        defaultFont->tileHeight = 12;
        defaultFont->tileSpacing = 1;

		fonts.emplace_back(defaultFont);

		//loadImage("../ProjectA/src/Asset/Texture/Quiver.png");
	}

	void GUILayer::loadTexture(const std::string& path) {
		ENGINE_ASSERT(false, "TODO: Load GUI texture.");

		//const ref<Image> image = renderer.loadImage(path);

		//images.emplace_back(texture);
	}

	void GUILayer::compute() {
		const vec2<float32> parentSize = {
			static_cast<float32>(Application::get().getSwapchain()->getExtentWidth()),
			static_cast<float32>(Application::get().getSwapchain()->getExtentHeight()),
		};

		const vec2<float32> parentMargin = {0.0f, 0.0f};
		chain<vec4<float32>> colors;

		/*for (const ref<GUIObject>& rootObject : rootObjects) {
			rootObject->compute(ecs, parentSize, parentMargin, colors, fonts[0]);
		}*/

		if (!colors.empty()) {
			renderer.registerColors(colors);
		}
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

	void GUILayer::update(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		updateScriptComponents();

		renderer.render(commandEncoder, nextImageIndex);
	}

	void GUILayer::updateScriptComponents() const {
		/*ComponentPool<ScriptComponent>& componentPool = ecs.getComponentPool<ScriptComponent>();

		ScriptComponent* begin = componentPool.components.data();

		for (uint32 componentIndex = 0; componentIndex < componentPool.componentCount; componentIndex += 1) {
			ScriptComponent* component = begin + componentIndex;

			if (component->entity == nullptr) {
				component->entity = component->constructScript();
				component->entity->onConstruct();
			}

			component->entity->onUpdate();
		}*/
	}
}