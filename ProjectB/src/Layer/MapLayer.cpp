// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Event/EventDispatcher.hpp"

#include "Application.hpp"
#include "Layer/MapLayer.hpp"

namespace ProjectB {
	void MapLayer::initialize() {
		renderer.setLayer(this);
		renderer.build();

		PerspectiveCamera3D& camera = cameraController.getCamera();

		// TODO(project-b/map/player-spawn)
		camera.position = {0.0f, 64.0f, 0.0f};

		camera.nearPlane = 0.01f;
		camera.farPlane = 2048.0f;
		camera.fieldOfView = radians(70.0_deg);
		camera.aspectRatio = getApplication()->getWindow()->getAspectRatio();

		// TODO(project-b/map/player-spawn)
		// camera.setOrientation(lookAt(FORWARD, sceneAsset->spawnDirection, UP));

		cameraController.updateProjection();

		camera.updateView();
		camera.updateProjectionView();
	}

	StaticMemoryDescriptor MapLayer::getStaticMemoryDescriptor() const {
		return renderer.getStaticMemoryDescriptor();
	}

	void MapLayer::encodeOneTimeCommands() {
		renderer.postBuild();
	}

	void MapLayer::update(float32 deltaTime, float32 inverseDeltaTime) {
		noclipMove(deltaTime);

		cameraController.update(deltaTime);
	}

	void MapLayer::render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const {
		renderer.render(commandEncoder);
		renderer.updateCamera(cameraController.getCamera());
	}

	void MapLayer::onEvent(Event& event) {
		EventDispatcher eventDispatcher(event);
		eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));
		eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
		eventDispatcher.dispatch<MouseMoveEvent>(BIND(onMouseMove));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));

		renderer.onEvent(event);
	}

	bool MapLayer::onKeyDown(KeyDownEvent& event) {
		const Window* window = getApplication()->getWindow();

		if (event.getKeyCode() == KeyCode::ESCAPE && window->isPointerLocked()) {
			window->unlockPointer();
		}

		return false;
	}

	bool MapLayer::onMouseDown(MouseDownEvent& event) {
		const ProjectB::Application* application = pointer_cast<const ProjectB::Application*>(getApplication());

		if (application->showConsole) {
			return true;
		}

		const Window* window = application->getWindow();

		if (!window->isPointerLocked()) {
			window->lockPointer();
		}

		return true;
	}

	bool MapLayer::onMouseMove(MouseMoveEvent& event) {
		if (!getApplication()->getWindow()->isPointerLocked()) {
			return true;
		}

		const vec3<float32> motion = vec3<float32>(vec3<sint32>{event.getY(), event.getX(), 0});

		cameraController.accumulateAngularMotion(radians(motion * MOUSE_SENSITIVITY));

		return true;
	}

	bool MapLayer::onWindowResize(WindowResizeEvent& event) {
		cameraController.getCamera().aspectRatio = getApplication()->getWindow()->getAspectRatio();
		cameraController.updateProjection();

		renderer.resize(event);

		return true;
	}

	void MapLayer::noclipMove(float32 deltaTime) {
		const ProjectB::Application* application = pointer_cast<const ProjectB::Application*>(getApplication());

		if (application->showConsole) {
			return;
		}

		PerspectiveCamera3D& camera = cameraController.getCamera();

		const Window* window = application->getWindow();
		const float32 forward = static_cast<float32>(window->isKeyDown(KeyCode::Z));
		const float32 back = static_cast<float32>(window->isKeyDown(KeyCode::S));
		const float32 right = static_cast<float32>(window->isKeyDown(KeyCode::D));
		const float32 left = static_cast<float32>(window->isKeyDown(KeyCode::Q));

		const vec3<float32> wishvel = camera.getForward() * (forward - back) + camera.getRight() * (right - left);

		if (lengthSquared(wishvel) == 0.0f) {
			return;
		}

		const vec3<float32> wishdir = normalized(wishvel) * MOVEMENT_SPEED * deltaTime;

		camera.position += wishdir;
	}
}