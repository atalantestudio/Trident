#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"

#include "BaseLayer.hpp"

namespace ProjectA {
	BaseLayer::BaseLayer() {
		visualizationData = {};
		physicsEngine.setScene(scene);
	}

	void BaseLayer::update(float32 deltaTime) {
		physicsEngine.tick(deltaTime, &visualizationData);
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

		renderer.clearRegisteredData();

		/* if (visualizationData.contactManifold.contactCount > 0) {
			Feature2D contactManifold;
			contactManifold.setVertexCount(visualizationData.contactManifold.contactCount);

			for (uint8 contactIndex = 0; contactIndex < contactManifold.getVertexCount(); contactIndex += 1) {
				contactManifold.edge[contactIndex] = visualizationData.contactManifold.contacts[contactIndex].position;
			}

			renderer.registerFeature(contactManifold);

			visualizationData.contactManifold.contactCount = 0;
		} */

		/* if (visualizationData.gjkOutput.closestFeatureA.count > 0) {
			renderer.setClosestPoints(visualizationData.gjkOutput.closestA, visualizationData.gjkOutput.closestB);
			renderer.clearRegisteredData();
			renderer.registerFeature(visualizationData.gjkOutput.closestFeatureA);
			renderer.registerFeature(visualizationData.gjkOutput.closestFeatureB);
		} */

		if (visualizationData.satOutput.separation <= 0.0f) {
			const vec2<float32>& closestA = visualizationData.satOutput.support;
			const vec2<float32> closestB = closestA - visualizationData.satOutput.normal * visualizationData.satOutput.separation;

			renderer.setClosestPoints(closestA, closestB);
			renderer.registerFeature(visualizationData.satOutput.closestFeatureA);
			renderer.registerFeature(visualizationData.satOutput.closestFeatureB);
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

		PhysicalObjectCopy2D copy;

		PhysicalObjectCopy2D p;
		p.position = renderer.getNdcMousePosition();
		p.geometry = createRef<PointGeometry2D>();
		p.transform = mat3x3<float32>::translation(p.position);

		// Check if an entity is hovered.
		for (sint32 entity = scene.getLastEntity(); entity >= 0; entity -= 1) {
			copy = copyPhysicalObject(scene, static_cast<Entity>(entity));

			if (GJK2DConditional(copy, p)) {
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

			// ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(scene.getBody(selectedEntity));

			// selectedEntityPreviousInverseMass = rigidBody->inverseMass;
			// selectedEntityPreviousInverseInertia = rigidBody->inverseInertia;

			// rigidBody->inverseMass = 0.0f;
			// rigidBody->inverseInertia = 0.0f;
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
			// rigidBody->inverseMass = selectedEntityPreviousInverseMass;
			// rigidBody->inverseInertia = selectedEntityPreviousInverseInertia;

			scene.updateTransform(selectedEntity);

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