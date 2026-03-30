// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/Tag.hpp"

#include "../../SampleApplication.hpp"
#include "BaseLayer.hpp"

namespace Trident {
	BaseLayer::BaseLayer() :
		physicsSimulation({
			.ecs = &ecs,
			.deltaTime = getApplication()->getFixedDeltaTime(),
		})
	{
		ecs.registerComponentPool<Geometry2D>();
		ecs.registerComponentPool<RigidBody2D>();
		ecs.registerComponentPool<ref<Constraint2D>>();
		ecs.registerComponentPool<Joint2D>();
		ecs.registerTagPool<StaticEntityTag>();
		ecs.registerTagPool<DynamicEntityTag>();
	}

	void BaseLayer::postConstruct() {
		renderer.layerInput = input;

		renderer.setLayer(this);
		renderer.build();
		renderer.setScene(sceneAsset, ecs);
	}

	void BaseLayer::update(float32 deltaTime, float32 inverseDeltaTime) {}

	void BaseLayer::preRender() {
		// TODO
		// For now there is 1 physics tick per frame because debugging along with Box2D Lite is easier.
		// Ultimately though this must be in Layer::update.
		physicsSimulation.tick();

		checkHover();

		const Scene2D& scene = pointer_cast<const SampleApplication*>(getApplication())->scene;
		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = scene.getComponentPool<RigidBody2D>();

		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const RigidBody2D& rigidBody = scene.getComponent<RigidBody2D>(entity);

			renderer.setRigidBodyTransform(entity, rigidBody.transform);

			componentIndex += 1;
		}

		renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<Entity>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		}

		renderer.clearRegisteredData();

		// TODO(physics/visualization-data)
		/* #if defined(BUILD_MODE_DEBUG)
			#if VISUALIZE_MODE == VISUALIZE_MODE_CONTACT_MANIFOLD
				if (visualizationData.contactManifold.contactCount > 0) {
					Feature2D contactManifold{};
					contactManifold.vertexCount = visualizationData.contactManifold.contactCount;

					uint8 contactIndex = 0;

					while (contactIndex < contactManifold.vertexCount) {
						contactManifold.edge[contactIndex] = visualizationData.contactManifold.contacts[contactIndex].position;

						contactIndex += 1;
					}

					renderer.registerFeature(contactManifold);
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
						vertexFeatureA.vertexCount = 1;
						vertexFeatureA.vertex = closestA;

						Feature2D vertexFeatureB{};
						vertexFeatureB.vertexCount = 1;
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
		#endif */
	}

	void BaseLayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		renderer.render(commandEncoder);
	}

	void BaseLayer::checkHover() {
		hoveredEntity = max<Entity>();

		const Scene2D& scene = pointer_cast<const SampleApplication*>(getApplication())->scene;
		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = scene.getComponentPool<RigidBody2D>();

		if (rigidBodyComponentPool.componentCount == 0) {
			return;
		}

		const Geometry2D mouseGeometry = Geometry2D::pointAtOrigin();

		RigidBody2D mouseRigidBody{};
		mouseRigidBody.position = mousePosition;
		mouseRigidBody.updateTransform(mouseGeometry.getCentroid());

		for (sint32 rigidBodyIndex = static_cast<sint32>(rigidBodyComponentPool.componentCount - 1); rigidBodyIndex >= 0; rigidBodyIndex -= 1) {
			const Entity entity = static_cast<Entity>(rigidBodyComponentPool.componentIndexToEntity[rigidBodyIndex]);
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(entity);
			const Geometry2D& geometry = sceneAsset->getGeometries()[geometryComponent.geometryIndex];
			const RigidBody2D& rigidBody = scene.getComponent<RigidBody2D>(entity);

			if (GJK2DConditional(rigidBody, mouseRigidBody, geometry, mouseGeometry)) {
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
		if (event.getButton() == MouseButton::LEFT && hoveredEntity != max<Entity>()) {
			selectedEntity = hoveredEntity;
			placeholderOffset = {0.0f, 0.0f};
		}

		return true;
	}

	bool BaseLayer::onMouseMove(MouseMoveEvent& event) {
		const float32 x = static_cast<float32>(event.getX());
		const float32 y = static_cast<float32>(event.getY());

		const vec2<float32> previousPosition = mousePosition;

		mousePosition = {x, y};

		if (selectedEntity != max<Entity>()) {
			const vec2<float32> position = mousePosition;
			const vec2<float32> positionDelta = position - previousPosition;

			placeholderOffset += positionDelta;
		}

		return true;
	}

	bool BaseLayer::onMouseUp(MouseUpEvent& event) {
		Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

		if (event.getButton() == MouseButton::LEFT && selectedEntity != max<Entity>()) {
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(selectedEntity);
			const Geometry2D& geometry = sceneAsset->getGeometries()[geometryComponent.geometryIndex];

			RigidBody2D& rigidBody = scene.getComponent<RigidBody2D>(selectedEntity);
			rigidBody.position += placeholderOffset;
			rigidBody.updateTransform(geometry.getCentroid());

			selectedEntity = max<Entity>();
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