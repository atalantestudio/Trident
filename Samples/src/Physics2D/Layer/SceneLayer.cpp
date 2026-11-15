// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/GJK/GJKConditional.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

#include "../../SampleApplication.hpp"
#include "SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::initialize() {
		rendererBuildOptions.sampleCount = ImageSampleCount::x1;
		rendererBuildOptions.cameraPosition = {0.0f, 8.0f};
		rendererBuildOptions.zoomLevel = 36.0f;
		rendererBuildOptions.maxContactCount = 2048;

		const view<char8> sceneAssetName = getApplication()->getPropertyFile()->getProperty<view<char8>>("AssetName");
		const ref<Scene2DAsset> sceneAsset = getApplication()->getAssetLibrary()->getAsset<Scene2DAsset>(sceneAssetName);

		scene = allocateRef<Scene2D>(sceneAsset);

		PhysicsSimulationDescriptor2D simulationDescriptor;
		simulationDescriptor.deltaTime = getApplication()->getFixedDeltaTime();
		simulationDescriptor.gravity = {0.0f, -9.80665f};
		simulationDescriptor.allowedPenetration = 0.01f;
		simulationDescriptor.impulseIterationCount = 10;
		simulationDescriptor.scene = scene;

		simulation = createSimulation(simulationDescriptor);

		renderer.setLayer(this);
		renderer.build();
		renderer.setScene(scene);
	}

	StaticMemoryDescriptor Physics2DSceneLayer::getStaticMemoryDescriptor() const {
		return renderer.getStaticMemoryDescriptor();
	}

	void Physics2DSceneLayer::postConstruct() {
		renderer.postBuild();
		renderer.encodeOneTimeCommands();
	}

	void Physics2DSceneLayer::preRender() {
		tick(simulation);

		checkHover();

		renderer.clearRegisteredData();

		uint32 shapeIndex = 0;

		while (shapeIndex < simulation.usedShapeCount) {
			const Shape2D& shape = simulation.shapes[shapeIndex];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			renderer.setWorld(shapeIndex, rigidBody.getWorld());

			shapeIndex += 1;
		}

		renderer.selectEntity(selectedEntity);

		if (selectedEntity != max<Entity>()) {
			renderer.setPlaceholderOffset(placeholderOffset);
		}

		if (draggingCamera) {
			renderer.updateCameraProjection();
		}
	}

	void Physics2DSceneLayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		renderer.render(commandEncoder);
	}

	void Physics2DSceneLayer::checkHover() {
		hoveredEntity = max<Entity>();

		if (simulation.usedShapeCount == 0) {
			return;
		}

		const vec2<float32> p = renderer.cameraPosition + mousePosition;

		sint32 shapeIndex = static_cast<sint32>(simulation.usedShapeCount) - 1;

		while (shapeIndex >= 0) {
			const Shape2D& shape = simulation.shapes[shapeIndex];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (GJKConditional2D(rigidBody, p, shape)) {
				hoveredEntity = shapeIndex;

				break;
			}

			shapeIndex -= 1;
		}
	}

	void Physics2DSceneLayer::registerHullForRender(const HullHeader2D* hullHeader) {
		PhysicsRendererBuildHull2D& hull = rendererBuildOptions.hulls[hullHeader->userIndex - 1];
		hull.hullHeader = hullHeader;
		hull.maxInstanceCount = 128;
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
			const Shape2D& shape = simulation.shapes[selectedEntity];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];
			rigidBody.position += placeholderOffset;

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