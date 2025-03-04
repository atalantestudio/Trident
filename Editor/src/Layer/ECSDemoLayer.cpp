#include "ECSDemoLayer.hpp"

namespace ProjectA {
	ECSDemoLayer::ECSDemoLayer() :
		Layer("ECSDemo"),
		triangleEntity(ecs.createEntity())
	{}

	void ECSDemoLayer::bind() {
		ecs.defineComponent<Transform2DComponent>();

		Transform2DComponent triangleTransformComponent;
		triangleTransformComponent.transform = mat3x3<float32>::identity();

		ecs.setComponent<Transform2DComponent>(triangleEntity, triangleTransformComponent);
	}

	void ECSDemoLayer::unbind() {
		const Transform2DComponent& triangleTransformComponent = ecs.getComponent<Transform2DComponent>(triangleEntity);

		CLIENT_DEBUG(triangleTransformComponent.transform);
	}

	void ECSDemoLayer::update(const float32 deltaTime) {}

	void ECSDemoLayer::renderImGui() {}

	void ECSDemoLayer::onEvent(Event& event) {}
}