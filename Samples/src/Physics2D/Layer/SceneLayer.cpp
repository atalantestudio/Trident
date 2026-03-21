// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/Constraint/RotationConstraint2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

#include "../../SampleApplication.hpp"
#include "SceneLayer.hpp"

namespace Trident {
	Physics2DSceneLayer::Physics2DSceneLayer() :
		physicsEngine(getPhysicsEngineDescriptor())
	{}

	PhysicsEngine2DDescriptor Physics2DSceneLayer::getPhysicsEngineDescriptor() {
		PhysicsEngine2DDescriptor descriptor{};
		descriptor.deltaTime = getApplication()->getFixedDeltaTime();

		#if defined(BUILD_MODE_DEBUG)
			descriptor.visualizationData = &visualizationData;
		#endif

		return descriptor;
	}

	void Physics2DSceneLayer::postConstruct() {
		Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

		const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

		RigidBody2DDescriptor floorDescriptor{};
		floorDescriptor.position = {0.0_m, -10.0_m};
		floorDescriptor.scale = {100.0f, 20.0f};
		floorDescriptor.friction = 1.0f;
		floorDescriptor.geometry = squareGeometry;
		floorDescriptor.fillColor = 0xf4fedbff;

		scene.createStaticRigidBody(floorDescriptor);

		RigidBody2DDescriptor playerDescriptor{};
		playerDescriptor.position = {0.0_m, 1.80_m * 0.5f};
		playerDescriptor.scale = {1.0_m, 1.80_m};
		playerDescriptor.mass = 1.0_kg;
		playerDescriptor.friction = 1.0f;
		playerDescriptor.geometry = squareGeometry;
		playerDescriptor.fillColor = 0xe6ddffff;

		scene.createDynamicRigidBody(playerDescriptor);

		playerEntity = scene.getLastEntity();

		RigidBody2DDescriptor obstacleDescriptor{};
		obstacleDescriptor.position = {5.0_m, 1.0_m};
		obstacleDescriptor.mass = 100000.0_kg;
		obstacleDescriptor.friction = 1.0f;
		obstacleDescriptor.geometry = squareGeometry;
		obstacleDescriptor.fillColor = 0xe6ddffff;

		scene.createDynamicRigidBody(obstacleDescriptor);

		RotationConstraint2DDescriptor constraintDescriptor{};
		constraintDescriptor.A = playerEntity;

		scene.createConstraint<RotationConstraint2D>(constraintDescriptor);

		physicsEngine.registerScene(scene);

		renderer.layerInput = input;
		renderer.setLayer(this);
		renderer.build();
		renderer.setScene(scene);
	}

	void Physics2DSceneLayer::update(float32 deltaTime, float32 inverseDeltaTime) {
		static constexpr float32 SPEED = 30.0f;
		static constexpr float32 JUMP_SPEED = 450.0f;

		Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;
		RigidBody2D& playerRigidBody = scene.getComponent<RigidBody2D>(playerEntity);

		if (keyMap[KeyCode::Q]) {
			playerRigidBody.applyImpulse({-SPEED * deltaTime, 0.0f});
		}

		if (keyMap[KeyCode::D]) {
			playerRigidBody.applyImpulse({SPEED * deltaTime, 0.0f});
		}

		if (keyMap[KeyCode::SPACE] && !jumping) {
			playerRigidBody.applyImpulse({0.0f, JUMP_SPEED * deltaTime});

			jumping = true;
		}

		if (abs(playerRigidBody.linearVelocity[1]) < 0.001f) {
			jumping = false;
		}
	}

	void Physics2DSceneLayer::preRender() {
		#if defined(BUILD_MODE_DEBUG)
			visualizationData = {};
		#endif

		physicsEngine.tick();

		checkHover();

		const Scene2D& scene = pointer_cast<const SampleApplication*>(getApplication())->scene;
		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = scene.getComponentPool<RigidBody2D>();

		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const RigidBody2D& rigidBody = scene.getComponent<RigidBody2D>(entity);

			renderer.setInstanceTransform(entity, rigidBody.transform);

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
		hoveredEntity = max<uint32>();

		const Scene2D& scene = pointer_cast<const SampleApplication*>(getApplication())->scene;
		const ComponentPool<RigidBody2D>& rigidBodyComponentPool = scene.getComponentPool<RigidBody2D>();

		if (rigidBodyComponentPool.componentCount == 0) {
			return;
		}

		RigidBody2D mouseRigidBody{};
		mouseRigidBody.position = renderer.getNdcMousePosition();
		mouseRigidBody.geometry = createRef<PointGeometry2D>();

		mouseRigidBody.updateTransform();

		for (sint32 rigidBodyIndex = static_cast<sint32>(rigidBodyComponentPool.componentCount - 1); rigidBodyIndex >= 0; rigidBodyIndex -= 1) {
			const Entity entity = static_cast<Entity>(rigidBodyComponentPool.componentIndexToEntity[rigidBodyIndex]);
			const RigidBody2D& rigidBody = scene.getComponent<RigidBody2D>(entity);

			if (GJK2DConditional(rigidBody, mouseRigidBody)) {
				hoveredEntity = entity;

				break;
			}
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
		if (event.getButton() == MouseButton::LEFT && hoveredEntity != max<uint32>()) {
			selectedEntity = hoveredEntity;
			placeholderOffset = {0.0f, 0.0f};
		}

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseMoveEvent& event) {
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

	bool Physics2DSceneLayer::onEvent(MouseUpEvent& event) {
		Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

		if (event.getButton() == MouseButton::LEFT && selectedEntity != max<uint32>()) {
			RigidBody2D& rigidBody = scene.getComponent<RigidBody2D>(selectedEntity);
			rigidBody.position += placeholderOffset;
			rigidBody.updateTransform();

			selectedEntity = max<uint32>();
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