#include "GUILayer.hpp"

namespace ProjectA {
	GUILayer::GUILayer() :
		Layer("GUILayer")
	{}

	void GUILayer::bind() {
		ecs.defineComponent<Transform2DComponent>();

		const Entity quad1Entity = ecs.createEntity();

		Transform2DComponent quad1TransformComponent;
		quad1TransformComponent.transform =
			mat3x3<float32>::scaling({64.0f, 64.0f}) *
			mat3x3<float32>::translation({16.0f, 0.0f});

		ecs.setComponent<Transform2DComponent>(quad1Entity, quad1TransformComponent);

		const Entity quad2Entity = ecs.createEntity();

		Transform2DComponent quad2TransformComponent;
		quad2TransformComponent.transform =
			mat3x3<float32>::scaling({128.0f, 48.0f}) *
			mat3x3<float32>::translation({16.0f, 80.0f});

		ecs.setComponent<Transform2DComponent>(quad2Entity, quad2TransformComponent);

		const Entity quad3Entity = ecs.createEntity();

		Transform2DComponent quad3TransformComponent;
		quad3TransformComponent.transform =
			mat3x3<float32>::scaling({256.0f, 512.0f}) *
			mat3x3<float32>::translation({80.0f, 128.0f});

		ecs.setComponent<Transform2DComponent>(quad3Entity, quad3TransformComponent);

		renderer.registerEntities(ecs);
	}

	void GUILayer::unbind() {}

	void GUILayer::update(const float32 deltaTime) {
		renderer.render();
	}

	void GUILayer::renderImGui() {}

	void GUILayer::onEvent(Event& event) {}
}