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
	Physics2DSceneLayer::~Physics2DSceneLayer() {
		delete scene;
		delete physicsSimulation;
	}

	void Physics2DSceneLayer::loadScene() {
		const Application* application = getApplication();
		const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene2DAsset> sceneAsset = application->getAssetLibrary()->getAsset<Scene2DAsset>(sceneAssetName);

		scene = new Scene2D(sceneAsset);
	}

	void Physics2DSceneLayer::postConstruct() {
		loadScene();

		physicsSimulation = new PhysicsSimulation2D({
			.scene = scene,
			.deltaTime = getApplication()->getFixedDeltaTime(),
		});

		renderer.layerInput = input;
		renderer.setLayer(this);
		renderer.build();
		renderer.setScene(scene);
	}

	void Physics2DSceneLayer::preRender() {
		physicsSimulation->tick();

		checkHover();

		const ComponentPool<RigidBody2DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody2DComponent>();

		uint32 componentIndex = 0;

		while (componentIndex < rigidBodyComponentPool.componentCount) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const RigidBody2DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

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
		mouseRigidBodyComponent.position = renderer.cameraPosition + mousePosition;
		mouseRigidBodyComponent.updateWorld(mouseHull.centroid);

		sint32 componentIndex = static_cast<sint32>(rigidBodyComponentPool.componentCount) - 1;

		while (componentIndex >= 0) {
			const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
			const Geometry2DComponent geometryComponent = scene->ecs.getComponent<Geometry2DComponent>(entity);
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
		const Extent2D extent = window->getExtent();

		mousePosition = window->getNdcMousePosition() * (vec2<float32>(extent) * 0.5f / renderer.getZoomLevel());
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
		const Extent2D extent = window->getExtent();
		const vec2<float32> previousMousePosition = mousePosition;

		mousePosition = window->getNdcMousePosition() * (vec2<float32>(extent) * 0.5f / renderer.getZoomLevel());
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
			const Geometry2DComponent geometryComponent = scene->ecs.getComponent<Geometry2DComponent>(selectedEntity);
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