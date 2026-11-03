// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Sample3D/Layer/Forward/ForwardLayer.hpp"

namespace Trident {
	void Sample3DForwardLayer::handleInputs() {
		const Window* window = getApplication()->getWindow();
		const PerspectiveCamera3D& camera = cameraController.getCamera();

		const float32 forwardKey = static_cast<float32>(window->isKeyDown(KeyCode::Z));
		const float32 backKey = static_cast<float32>(window->isKeyDown(KeyCode::S));
		const float32 rightKey = static_cast<float32>(window->isKeyDown(KeyCode::D));
		const float32 leftKey = static_cast<float32>(window->isKeyDown(KeyCode::Q));

		const vec3<float32> right = camera.getRight();
		const vec3<float32> forward = cross(right, UP);

		desiredVelocity = forward * (PLAYER_SPEED * (forwardKey - backKey)) + right * (PLAYER_SPEED * (rightKey - leftKey));
	}
}