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
		AssetLibrary* assetLibrary = getApplication()->getAssetLibrary();

		// TODO
		// The geometry assets used by the scene should be loaded along with the scene asset.
		// For now they must be loaded manually before loading the scene.
		assetLibrary->getAsset<GeometryAsset>("UnitCircle");
		assetLibrary->getAsset<GeometryAsset>("UnitSquare");
		assetLibrary->getAsset<GeometryAsset>("Triangle");
		assetLibrary->getAsset<GeometryAsset>("Polygon");

		const view<char8> sceneAssetName = getApplication()->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene2DAsset> sceneAsset = assetLibrary->getAsset<Scene2DAsset>(sceneAssetName);

		scene = createScope<Scene2D>(sceneAsset);

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

		playerEntity = scene.ecs.nextEntity - 1;

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

	void Physics2DSceneLayer::postConstruct() {
		loadScene();

		physicsSimulation = createScope<PhysicsSimulation2D>(Descriptor<PhysicsSimulation2D>{
			.scene = scene.get(),
			.deltaTime = getApplication()->getFixedDeltaTime(),
		});

		renderer.layerInput = input;
		renderer.setLayer(this);
		renderer.build();
		renderer.setScene(scene.get());
	}

	void Physics2DSceneLayer::update(float32 deltaTime, float32 inverseDeltaTime) {
		/* constexpr float32 SPEED = 3.0f;

		RigidBody2DComponent& playerRigidBody = ecs.getComponent<RigidBody2DComponent>(playerEntity);

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
		physicsSimulation->tick();

		checkHover();

		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();

		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const RigidBody2DComponent& rigidBodyComponent = scene->ecs.getComponent<RigidBody2DComponent>(entity);

			renderer.setWorld(entity, rigidBodyComponent.world);

			componentIndex += 1;
		}

		renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<Entity>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		}

		if (draggingCamera) {
			renderer.updateCameraProjection();
		}

		renderer.clearRegisteredData();
	}

	void Physics2DSceneLayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		renderer.render(commandEncoder);
	}

	void Physics2DSceneLayer::checkHover() {
		hoveredEntity = max<Entity>();

		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();

		if (rigidBodyComponentPool.componentCount == 0) {
			return;
		}

		const Hull2D mouseHull = Hull2D::point();

		RigidBody2DComponent mouseRigidBodyComponent{};
		mouseRigidBodyComponent.position = svec2<float32>(renderer.cameraPosition + mousePosition);
		mouseRigidBodyComponent.updateWorld(mouseHull.centroid);

		sint32 componentIndex = static_cast<sint32>(rigidBodyComponentPool.componentCount) - 1;

		while (componentIndex >= 0) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const GeometryComponent geometryComponent = scene->ecs.getComponent<GeometryComponent>(entity);
			const Geometry2D& geometry = scene->getGeometries()[geometryComponent.geometryIndex];
			const Hull2D& hull = scene->getHulls()[geometry.hullIndex];
			const RigidBody2DComponent& rigidBodyComponent = scene->ecs.getComponent<RigidBody2DComponent>(entity);

			if (GJK2DConditional(rigidBodyComponent, mouseRigidBodyComponent, hull, mouseHull)) {
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
		const Window* window = getApplication()->getWindow();
		const sint32* size = window->getSize();

		mousePosition = window->getNdcMousePosition() * (vec2<float32>(static_cast<float32>(size[0]), static_cast<float32>(size[1])) * 0.5f / renderer.getZoomLevel());
		mouseDelta = {0.0f, 0.0f};

		if (event.getButton() == MouseButton::RIGHT && selectedEntity == max<Entity>()) {
			draggingCamera = true;
		}

		if (event.getButton() == MouseButton::LEFT && hoveredEntity != max<Entity>() && !draggingCamera) {
			selectedEntity = hoveredEntity;
			placeholderOffset = {0.0f, 0.0f};
		}

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseMoveEvent& event) {
		const Window* window = getApplication()->getWindow();
		const sint32* size = window->getSize();
		const vec2<float32> previousMousePosition = mousePosition;

		mousePosition = window->getNdcMousePosition() * (vec2<float32>(static_cast<float32>(size[0]), static_cast<float32>(size[1])) * 0.5f / renderer.getZoomLevel());
		mouseDelta = mousePosition - previousMousePosition;

		if (draggingCamera) {
			renderer.cameraPosition -= mouseDelta;
		}

		if (selectedEntity != max<Entity>()) {
			placeholderOffset += mouseDelta;
		}

		return true;
	}

	bool Physics2DSceneLayer::onEvent(MouseUpEvent& event) {
		if (event.getButton() == MouseButton::RIGHT && draggingCamera) {
			draggingCamera = false;
		}

		if (event.getButton() == MouseButton::LEFT && selectedEntity != max<Entity>()) {
			const GeometryComponent geometryComponent = scene->ecs.getComponent<GeometryComponent>(selectedEntity);
			const Geometry2D& geometry = scene->getGeometries()[geometryComponent.geometryIndex];
			const Hull2D& hull = scene->getHulls()[geometry.hullIndex];

			RigidBody2DComponent& rigidBody = scene->ecs.getComponent<RigidBody2DComponent>(selectedEntity);
			rigidBody.position += placeholderOffset;
			rigidBody.updateWorld(hull.centroid);

			selectedEntity = max<Entity>();
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