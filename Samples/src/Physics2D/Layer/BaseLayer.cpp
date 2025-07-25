#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"

#include "BaseLayer.hpp"

namespace ProjectA {
	BaseLayer::BaseLayer() :
		physicsEngine(getPhysicsEngineDescriptor())
	{}

	void BaseLayer::postConstruct() {
		renderer.build();
		renderer.registerScene(scene);

		physicsEngine.registerScene(scene);
	}

	void BaseLayer::update(float32 deltaTime, float32 inverseDeltaTime) {
		physicsEngine.tick();
	}

	void BaseLayer::preRender() {
		checkHover();

		// TODO: Only write once the static component transforms.
		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = scene.getComponentPool<RigidBody2D>();

		for (uint32 rigidBodyIndex = 0; rigidBodyIndex < rigidBodyComponentPool.componentCount; rigidBodyIndex += 1) {
			const Entity entity = static_cast<Entity>(rigidBodyComponentPool.componentIndexToEntity[rigidBodyIndex]);
			const RigidBody2D& rigidBody = scene.getRigidBody(entity);

			renderer.writeInstanceTransform(rigidBodyIndex, rigidBody.transform);
		}

		renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<uint32>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		}

		// renderer.clearRegisteredData();

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

		/* if (visualizationData.satOutput.separation <= 0.0f) {
			const vec2<float32>& closestA = visualizationData.satOutput.incidentSupportPoint;
			const vec2<float32> closestB = closestA - visualizationData.satOutput.normal * visualizationData.satOutput.separation;

			renderer.setClosestPoints(closestA, closestB);
			renderer.registerFeature(visualizationData.satOutput.referenceEdge);
		} */
	}

	void BaseLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}

	PhysicsEngine2DDescriptor BaseLayer::getPhysicsEngineDescriptor() {
		PhysicsEngine2DDescriptor descriptor{};
		descriptor.deltaTime = Application::getFixedDeltaTime();

		#ifdef BUILD_MODE_DEBUG
			descriptor.visualizationData = &visualizationData;
		#endif

		return descriptor;
	}

	void BaseLayer::checkHover() {
		hoveredEntity = max<uint32>();

		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = scene.getComponentPool<RigidBody2D>();

		if (rigidBodyComponentPool.componentCount == 0) {
			return;
		}

		RigidBody2D mouseRigidBody{};
		mouseRigidBody.position = renderer.getNdcMousePosition();
		mouseRigidBody.geometry = createRef<PointGeometry2D>();

		mouseRigidBody.updateTransform();

		// Check if an entity is hovered.
		// for (sint32 entity = scene.getLastEntity(); entity >= 0; entity -= 1) {
		for (sint32 rigidBodyIndex = static_cast<sint32>(rigidBodyComponentPool.componentCount - 1); rigidBodyIndex >= 0; rigidBodyIndex -= 1) {
			const Entity entity = static_cast<Entity>(rigidBodyComponentPool.componentIndexToEntity[rigidBodyIndex]);
			const RigidBody2D& rigidBody = scene.getRigidBody(entity);

			if (GJK2DConditional(rigidBody, mouseRigidBody)) {
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

			// RigidBody2D& rigidBody = scene.getRigidBody(selectedEntity);

			// selectedEntityPreviousInverseMass = rigidBody.inverseMass;
			// selectedEntityPreviousInverseInertia = rigidBody.inverseInertia;

			// rigidBody.inverseMass = 0.0f;
			// rigidBody.inverseInertia = 0.0f;
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
			RigidBody2D& rigidBody = scene.getRigidBody(selectedEntity);

			rigidBody.position += placeholderOffset;
			// rigidBody.inverseMass = selectedEntityPreviousInverseMass;
			// rigidBody.inverseInertia = selectedEntityPreviousInverseInertia;

			rigidBody.updateTransform();

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