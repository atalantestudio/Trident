#include "pch.hpp"

#include "Engine/API/Physics/Integrator.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

#include "BaseLayer.hpp"

namespace ProjectA {
	void BaseLayer::update(float64 fixedDeltaTime) {
		for (Entity entity = 0; entity < scene.getPhysicalObjectEntityCount(); entity += 1) {
			ref<RigidBody2D>& body = std::static_pointer_cast<RigidBody2D>(scene.getBody(entity));

			body->linearForces = {0.0f, 0.0f};
		}

		PhysicsUpdate();

		Integrator::integrate(scene, &gjkResult);
	}

	void BaseLayer::preRender() {
		checkHover();

		for (Entity entity = 0; entity < scene.getPhysicalObjectEntityCount(); entity += 1) {
			const PhysicalObject2D& physicalObject = scene.getPhysicalObject(entity);

			renderer.writeInstanceTransform(entity, physicalObject.transform);
		}

		renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<uint32>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		}

		if (gjkResult.closestFeatureA.count > 0) {
			renderer.setClosestPoints(gjkResult.closestA, gjkResult.closestB);
			renderer.clearClosestFeatureCounters();
			renderer.registerClosestFeature(gjkResult.closestFeatureA);
			renderer.registerClosestFeature(gjkResult.closestFeatureB);
		}
	}

	void BaseLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	void BaseLayer::checkHover() {
		hoveredEntity = max<uint32>();

		if (scene.getPhysicalObjectEntityCount() == 0) {
			return;
		}

		const vec2<float32>& p = renderer.getNdcMousePosition();

		PhysicalObjectCopy2D copy;

		// Check if the mouse hovers over an object.
		for (sint32 entity = scene.getLastEntity(); entity >= 0; entity -= 1) {
			copy = copyPhysicalObject(scene, static_cast<Entity>(entity));

			if (GJK2DPoint(copy, p).distance <= 0.0f) {
				hoveredEntity = entity;

				break;
			}
		}
	}

	void BaseLayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
		eventDispatcher.dispatch<MouseMoveEvent>(BIND(onMouseMove));
		eventDispatcher.dispatch<MouseUpEvent>(BIND(onMouseUp));
		eventDispatcher.dispatch<MouseWheelEvent>(BIND(onMouseWheel));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
	}

	bool BaseLayer::onMouseDown(MouseDownEvent& event) {
		if (event.getButton() == MouseButton::LEFT && hoveredEntity != max<uint32>()) {
			selectedEntity = hoveredEntity;
			placeholderOffset = {0.0f, 0.0f};
		}

		return true;
	}

	bool BaseLayer::onMouseMove(MouseMoveEvent& event) {
		const float32 x = static_cast<float32>(event.getX());
		const float32 y = static_cast<float32>(event.getY());

		const vec2<float32> previousPosition = renderer.getNdcMousePosition();

		renderer.setMousePosition(x, y);

		if (selectedEntity != max<uint32>()) {
			const vec2<float32>& position = renderer.getNdcMousePosition();
			const vec2<float32> positionDelta = position - previousPosition;

			placeholderOffset += positionDelta;
		}

		return true;
	}

	bool BaseLayer::onMouseUp(MouseUpEvent& event) {
		if (event.getButton() == MouseButton::LEFT && selectedEntity != max<uint32>()) {
			ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene.getBody(selectedEntity));

			rigidBody->position += placeholderOffset;

			selectedEntity = max<uint32>();
		}

		return true;
	}

	bool BaseLayer::onMouseWheel(MouseWheelEvent& event) {
		renderer.zoom(static_cast<float32>(event.getDelta()));

		return true;
	}

	bool BaseLayer::onWindowResize(WindowResizeEvent& event) {
		renderer.resize(event);

		return false;
	}
}