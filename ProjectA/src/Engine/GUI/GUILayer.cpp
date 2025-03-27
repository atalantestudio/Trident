#include "pch.hpp"

#include "Engine/ECS/Component/GUIComponent.hpp"
#include "Engine/ECS/Component/ScriptComponent.hpp"
#include "Engine/GUI/Font/BitmapFont.hpp"
#include "Engine/GUI/Font/GlyphMap.hpp"
#include "Engine/GUI/GUILayer.hpp"

namespace ProjectA {
	GUILayer::GUILayer() {
		ecs.registerComponent<GUIComponent>();
		ecs.registerComponent<ScriptComponent>();

		loadDefaultTexture();
	}

	GUILayer::~GUILayer() {
		ComponentPool<ScriptComponent>& componentPool = ecs.getComponentPool<ScriptComponent>();

		ScriptComponent* begin = componentPool.components.data();

		for (uint32 componentIndex = 0; componentIndex < componentPool.componentCount; componentIndex += 1) {
			ScriptComponent* component = begin + componentIndex;

			component->entity->onDestroy();
			component->destroyScript(component);
		}
	}

	void GUILayer::loadDefaultFont() {
		const ref<BitmapFont> defaultFont = createRef<BitmapFont>();
		defaultFont->textureIndex = static_cast<uint32>(textures.size());
		defaultFont->glyphMap.map = QUIVER_GLYPH_MAP;
        defaultFont->glyphMap.base = 32;
        defaultFont->glyphMap.count = 220;
        defaultFont->tileHeight = 12;
        defaultFont->tileSpacing = 1;

		fonts.emplace_back(defaultFont);

		loadTexture("../ProjectA/src/Asset/Texture/Quiver.png");
	}

	void GUILayer::loadTexture(const std::string& path) {
		const ref<Texture> texture = renderer.loadTexture(path);

		textures.emplace_back(texture);
	}

	void GUILayer::append(const GUIObject& object) {
		for (const GUISubcomponent& subcomponent : object.getSubcomponents()) {
			const Entity entity = ecs.createEntity();

			GUIComponent component;
			component.transform = mat3x3<float32>::scaling(subcomponent.size) * mat3x3<float32>::translation(object.getPosition() + subcomponent.offset);
			component.colorIndex = object.getColorIndex();
			component.textureIndex = object.getTextureIndex();

			ecs.setComponent<GUIComponent>(entity, component);
		}
	}

	void GUILayer::registerColors(const chain<vec4<float32>>& colors) const {
		renderer.registerColors(colors);
	}

	void GUILayer::registerEntityPool() {
		const ComponentPool<GUIComponent>& componentPool = ecs.getComponentPool<GUIComponent>();

		ENGINE_ASSERT(componentPool.componentCount < MAX_GUI_COMPONENTS, "Too many GUI components.");

		if (componentPool.componentCount == 0) {
			ENGINE_WARN("No GUI component registered.");

			return;
		}

		renderer.registerEntityPool(componentPool);
	}

	void GUILayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		updateScriptComponents();

		renderer.render(commandEncoder, nextImageIndex);
	}

	void GUILayer::updateScriptComponents() const {
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
	}

	void GUILayer::loadDefaultTexture() {
		const ref<Texture> texture = renderer.loadDefaultTexture();

		textures.emplace_back(texture);
	}
}