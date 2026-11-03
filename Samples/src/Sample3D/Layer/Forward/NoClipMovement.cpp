// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Sample3D/Layer/Forward/ForwardLayer.hpp"

namespace Trident {
	void Sample3DForwardLayer::noclipMove(float32 deltaTime) {
		const Window* window = getApplication()->getWindow();

		PerspectiveCamera3D& camera = cameraController.getCamera();

		const float32 forward = static_cast<float32>(window->isKeyDown(KeyCode::Z));
		const float32 back = static_cast<float32>(window->isKeyDown(KeyCode::S));
		const float32 right = static_cast<float32>(window->isKeyDown(KeyCode::D));
		const float32 left = static_cast<float32>(window->isKeyDown(KeyCode::Q));

		const vec3<float32> wishvel = camera.getForward() * (forward - back) + camera.getRight() * (right - left);

		if (lengthSquared(wishvel) == 0.0f) {
			return;
		}

		const vec3<float32> wishdir = normalized(wishvel) * MOVEMENT_SPEED * deltaTime;

		RigidBody3DComponent& player = scene->ecs.getComponent<RigidBody3DComponent>(playerEntity);
		player.position += wishdir * 40.0f;
		player.updateWorld({0.0f, 0.0f, 0.0f});
	}
}