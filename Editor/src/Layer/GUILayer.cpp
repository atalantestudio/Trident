#include "GUILayer.hpp"

namespace ProjectA {
	GUILayer::GUILayer() :
		Layer("GUILayer")
	{}

	void GUILayer::bind() {
		ecs.defineComponent<GUIComponent>();

		const Entity quad1Entity = ecs.createEntity();

		GUIComponent quad1GuiComponent;
		quad1GuiComponent.transform =
			mat3x3<float32>::scaling({64.0f, 64.0f}) *
			mat3x3<float32>::translation({0.0f, 0.0f});
		quad1GuiComponent.colorIndex = 0;
		quad1GuiComponent.textureIndex = 0;

		ecs.setComponent<GUIComponent>(quad1Entity, quad1GuiComponent);

		const Entity quad2Entity = ecs.createEntity();

		GUIComponent quad2GuiComponent;
		quad2GuiComponent.transform =
			mat3x3<float32>::scaling({128.0f, 48.0f}) *
			mat3x3<float32>::translation({16.0f, 80.0f});
		quad2GuiComponent.colorIndex = 1;
		quad2GuiComponent.textureIndex = 0;

		ecs.setComponent<GUIComponent>(quad2Entity, quad2GuiComponent);

		const Entity quad3Entity = ecs.createEntity();

		GUIComponent quad3GuiComponent;
		quad3GuiComponent.transform =
			mat3x3<float32>::scaling({256.0f, 512.0f}) *
			mat3x3<float32>::translation({80.0f, 128.0f});
		quad3GuiComponent.colorIndex = 2;
		quad3GuiComponent.textureIndex = 0;

		ecs.setComponent<GUIComponent>(quad3Entity, quad3GuiComponent);

		renderer.registerEntities(ecs);

		const std::vector<vec4<float32>> colors = {
			{1.0f, 1.0f, 1.0f, 1.0f},
			{1.0f, 0.2f, 0.0f, 1.0f},
			{0.1f, 0.1f, 0.2f, 1.0f},
		};

		renderer.registerColors(colors);
	}

	void GUILayer::unbind() {}

	void GUILayer::update(const float32 deltaTime) {
		renderer.render();
	}

	void GUILayer::renderImGui() {}

	void GUILayer::onEvent(Event& event) {}
}