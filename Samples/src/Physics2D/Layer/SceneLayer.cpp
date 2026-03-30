// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/Constraint/RotationConstraint2D.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/Tag.hpp"

#include "../../SampleApplication.hpp"
#include "SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::loadScene() {
		sceneAsset = getApplication()->getAssetLibrary()->getAsset<Scene2DAsset>("Scene");

		const view<GeometryComponent> geometryComponents = sceneAsset->getGeometryComponents();

		Entity entity = 0;

		while (entity < sceneAsset->getEntityCount()) {
			ecs.createEntity();
			ecs.setComponent<GeometryComponent>(entity, geometryComponents[entity]);

			entity += 1;
		}

		for (const CachedRigidBody2D& rigidBody : sceneAsset->getRigidBodies()) {
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(rigidBody.entity);
			const Geometry2D& geometry = sceneAsset->getGeometries()[geometryComponent.geometryIndex];

			ecs.setComponent<RigidBody2D>(rigidBody.entity, RigidBody2D::fromCached(rigidBody, geometry.getCentroid()));
			ecs.setTag<DynamicEntityTag>(rigidBody.entity);
		}

		// TODO(physics/samples): Restore sample.
		/* const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

		// Floor
		scene.createStaticRigidBody({
			.position = {0_m, -0.5_m},
			.scale = {16.0f, 1.0f},
			.friction = 1.0f,
			.geometry = squareGeometry,
			.fillColor = 0xf4fedbff,
		});

		// Left wall
		scene.createStaticRigidBody({
			.position = {-7.75_m, 0.6_m},
			.scale = {0.5f, 1.2f},
			.friction = 1.0f,
			.geometry = squareGeometry,
			.fillColor = 0xf4fedbff,
		});

		// Right wall
		scene.createStaticRigidBody({
			.position = {7.75_m, 2_m},
			.scale = {0.5f, 4.0f},
			.friction = 1.0f,
			.geometry = squareGeometry,
			.fillColor = 0xf4fedbff,
		});

		// Heightened floor
		scene.createStaticRigidBody({
			.position = {7_m, 0.1_m},
			.scale = {1.0f, 0.2f},
			.friction = 1.0f,
			.geometry = squareGeometry,
			.fillColor = 0xf4fedbff,
		});

		// Player
		scene.createDynamicRigidBody({
			.position = {0_m, 1.8_m * 0.5f},
			.scale = {1_m, 1.8_m},
			.mass = 1_kg,
			.friction = 1.0f,
			.geometry = squareGeometry,
			.fillColor = 0xe6ddffff,
		});

		playerEntity = scene.getLastEntity();

		// Prop box
		scene.createDynamicRigidBody({
			.position = {4_m, 1_m},
			.scale = {2.5f, 0.5f},
			.mass = 1_kg,
			.friction = 1.0f,
			.geometry = squareGeometry,
			.fillColor = 0xe6ddffff,
		});

		RotationConstraint2DDescriptor constraintDescriptor{};
		constraintDescriptor.A = playerEntity;

		scene.createConstraint<RotationConstraint2D>(constraintDescriptor); */
	}

	Physics2DSceneLayer::Physics2DSceneLayer() :
		physicsSimulation({
			.ecs = &ecs,
			.deltaTime = getApplication()->getFixedDeltaTime(),
		})
	{
		ecs.registerComponentPool<GeometryComponent>();

		// TODO: MaterialReferenceComponent, RigidBody2DComponent, RigidBody3DComponent. Transform2DComponent/Transform3DComponent for static meshes?

		ecs.registerComponentPool<RigidBody2D>();
		ecs.registerComponentPool<ref<Constraint2D>>();
		ecs.registerComponentPool<Joint2D>();

		ecs.registerTagPool<StaticEntityTag>();
		ecs.registerTagPool<DynamicEntityTag>();
	}

	void Physics2DSceneLayer::postConstruct() {
		loadScene();

		physicsSimulation.sceneAsset = sceneAsset;

		renderer.layerInput = input;
		renderer.setLayer(this);
		renderer.build();
		renderer.setScene(sceneAsset, ecs);
	}

	void Physics2DSceneLayer::update(float32 deltaTime, float32 inverseDeltaTime) {
		/* constexpr float32 SPEED = 3.0f;

		RigidBody2D& playerRigidBody = ecs.getComponent<RigidBody2D>(playerEntity);

		if (keyMap[KeyCode::Q]) {
			playerRigidBody.linearVelocity += {-SPEED, playerRigidBody.linearVelocity[1]};
		}

		if (keyMap[KeyCode::D]) {
			playerRigidBody.linearVelocity += {SPEED, playerRigidBody.linearVelocity[1]};
		}

		if (keyMap[KeyCode::SPACE] && !jumping) {
			const float32 jumpForce = sqrt(2.0f * 800.0f * 45.0f);

			playerRigidBody.linearVelocity = {playerRigidBody.linearVelocity[0], jumpForce * deltaTime};

			jumping = true;
		}

		if (abs(playerRigidBody.linearVelocity[1]) < 0.001f) {
			jumping = false;
		} */
	}

	void Physics2DSceneLayer::preRender() {
		physicsSimulation.tick();

		if (dragging) {
			renderer.updateCameraProjection();
		}

		checkHover();

		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = ecs.getComponentPool<RigidBody2D>();

		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const RigidBody2D& rigidBody = ecs.getComponent<RigidBody2D>(entity);

			renderer.setRigidBodyTransform(entity, rigidBody.transform);

			componentIndex += 1;
		}

		renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<uint32>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		}

		renderer.clearRegisteredData();
	}

	void Physics2DSceneLayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		renderer.render(commandEncoder);
	}

	void Physics2DSceneLayer::checkHover() {
		hoveredEntity = max<Entity>();

		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = ecs.getComponentPool<RigidBody2D>();

		if (rigidBodyComponentPool.componentCount == 0) {
			return;
		}

		const Geometry2D mouseGeometry = Geometry2D::pointAtOrigin();

		RigidBody2D mouseRigidBody{};

		// TODO: Include camera position.
		mouseRigidBody.position = renderer.getNdcMousePosition() - renderer.cameraPosition;

		mouseRigidBody.updateTransform(mouseGeometry.getCentroid());

		sint32 componentIndex = static_cast<sint32>(rigidBodyComponentPool.componentCount) - 1;

		while (componentIndex >= 0) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(entity);
			const Geometry2D& geometry = sceneAsset->getGeometries()[geometryComponent.geometryIndex];
			const RigidBody2D& rigidBody = ecs.getComponent<RigidBody2D>(entity);

			if (GJK2DConditional(rigidBody, mouseRigidBody, geometry, mouseGeometry)) {
				hoveredEntity = entity;

				break;
			}

			componentIndex -= 1;
		}
	}

	void Physics2DSceneLayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<KeyDownEvent>(BIND(onEvent));
		eventDispatcher.dispatch<KeyUpEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseDownEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseMoveEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseUpEvent>(BIND(onEvent));
		eventDispatcher.dispatch<MouseWheelEvent>(BIND(onEvent));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onEvent));
	}

	bool Physics2DSceneLayer::onEvent(KeyDownEvent& event) {
		keyMap.update(event);

		return true;
	}

	bool Physics2DSceneLayer::onEvent(KeyUpEvent& event) {
		keyMap.update(event);

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseDownEvent& event) {
		if (event.getButton() == MouseButton::LEFT && hoveredEntity != max<Entity>()) {
			selectedEntity = hoveredEntity;
			placeholderOffset = {0.0f, 0.0f};
		}

		if (event.getButton() == MouseButton::RIGHT && !dragging) {
			dragging = true;
		}

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseMoveEvent& event) {
		const float32 x = static_cast<float32>(event.getX());
		const float32 y = static_cast<float32>(event.getY());
		const vec2<float32> previousPosition = renderer.getNdcMousePosition();

		renderer.setMousePosition(x, y);

		const vec2<float32> position = renderer.getNdcMousePosition();
		const vec2<float32> positionDelta = position - previousPosition;

		if (selectedEntity != max<uint32>()) {
			placeholderOffset += positionDelta;
		}

		if (dragging) {
			// TODO(grid-drag): (20, 11.25) is used because the ratio is 16/9.
			renderer.cameraPosition += positionDelta / vec2<float32>(20.0f, 11.25f);
		}

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseUpEvent& event) {
		if (event.getButton() == MouseButton::LEFT && selectedEntity != max<uint32>()) {
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(selectedEntity);
			const Geometry2D& geometry = sceneAsset->getGeometries()[geometryComponent.geometryIndex];

			RigidBody2D& rigidBody = ecs.getComponent<RigidBody2D>(selectedEntity);
			rigidBody.position += placeholderOffset;
			rigidBody.updateTransform(geometry.centroid);

			selectedEntity = max<uint32>();
		}

		if (event.getButton() == MouseButton::RIGHT && dragging) {
			dragging = false;
		}

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseWheelEvent& event) {
		renderer.zoom(static_cast<float32>(event.getDelta()));

		return true;
	}

	bool Physics2DSceneLayer::onEvent(WindowResizeEvent& event) {
		renderer.resize(event);

		return false;
	}
}