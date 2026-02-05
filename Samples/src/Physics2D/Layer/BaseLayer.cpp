// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"

#include "BaseLayer.hpp"

namespace Trident {
	BaseLayer::BaseLayer() :
		physicsEngine(getPhysicsEngineDescriptor())
		#if defined(BUILD_MODE_DEBUG)
			, visualizationData{}
		#endif
	{}

	void BaseLayer::postConstruct() {
		renderer.layerInput = input;

		renderer.setLayer(this);
		renderer.build();
		renderer.setScene(scene);

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

			renderer.setInstanceTransform(rigidBodyIndex, rigidBody.transform);
		}

		renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<uint32>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		}

		renderer.clearRegisteredData();

		#if VISUALIZE_MODE == VISUALIZE_MODE_CONTACT_MANIFOLD
			if (visualizationData.contactManifold.getContactCount() > 0) {
				Feature2D contactManifold{};
				contactManifold.setVertexCount(visualizationData.contactManifold.getContactCount());

				for (uint8 contactIndex = 0; contactIndex < contactManifold.getVertexCount(); contactIndex += 1) {
					contactManifold.edge[contactIndex] = visualizationData.contactManifold.contacts[contactIndex].position;
				}

				#if defined(SHOW_FEATURES)
					renderer.registerFeature(contactManifold);
				#endif
			}
		#endif

		#if VISUALIZE_MODE == VISUALIZE_MODE_CLOSEST_POINTS
			if (visualizationData.satOutput.separation > 0.0f) {
				const vec2<float32>& closestA = visualizationData.satOutput.incidentSupportPoint;
				const vec2<float32> closestB = closestA - visualizationData.satOutput.normal * visualizationData.satOutput.separation;

				#if defined(SHOW_DISTANCE)
					renderer.setClosestPoints(closestA, closestB);
				#endif

				#if defined(SHOW_FEATURES)
					Feature2D vertexFeatureA{};
					vertexFeatureA.setVertexCount(1);
					vertexFeatureA.vertex = closestA;

					Feature2D vertexFeatureB{};
					vertexFeatureB.setVertexCount(1);
					vertexFeatureB.vertex = closestB;

					renderer.registerFeature(vertexFeatureA);
					renderer.registerFeature(vertexFeatureB);
				#endif
			}
		#endif

		#if VISUALIZE_MODE == VISUALIZE_MODE_REFERENCE_EDGE_TOUCHING
			if (visualizationData.satOutput.separation <= 0.0f) {
				#if defined(SHOW_FEATURES)
					renderer.registerFeature(visualizationData.satOutput.referenceEdge);
				#endif
			}
		#endif
	}

	void BaseLayer::render(float64 remainingFrameTime, ref<CommandEncoder> commandEncoder) const {
		renderer.render(commandEncoder);
	}

	PhysicsEngine2DDescriptor BaseLayer::getPhysicsEngineDescriptor() {
		PhysicsEngine2DDescriptor descriptor{};
		descriptor.deltaTime = Application::getFixedDeltaTime();

		#if defined(BUILD_MODE_DEBUG)
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

			// rigidBody.inverseMass = 0.0_kg;
			// rigidBody.inverseInertia = 0.0_kg_m2;
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