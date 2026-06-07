// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "ForwardLayer.hpp"

namespace Trident {
	void Sample3DForwardLayer::handleInputs() {
		const Window* window = getApplication()->getWindow();
		const PerspectiveCamera3D& camera = cameraController.getCamera();

		const float32 forwardKey = static_cast<float32>(window->isKeyDown(KeyCode::Z));
		const float32 backKey = static_cast<float32>(window->isKeyDown(KeyCode::S));
		const float32 rightKey = static_cast<float32>(window->isKeyDown(KeyCode::D));
		const float32 leftKey = static_cast<float32>(window->isKeyDown(KeyCode::Q));

		const vec3<float32> forward = normalized(camera.getForward() * vec3<float32>{1.0f, 0.0f, 1.0f});
		const vec3<float32> right = normalized(camera.getRight() * vec3<float32>{1.0f, 0.0f, 1.0f});

		desiredVelocity = forward * (PLAYER_SPEED * (forwardKey - backKey)) + right * (PLAYER_SPEED * (rightKey - leftKey));
	}

	void Sample3DForwardLayer::playerMove(float32 deltaTime) {
		const Window* window = getApplication()->getWindow();

		RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

		playerRigidBody.linearVelocity += PLAYER_GRAVITY * deltaTime * 0.5f;

		if (window->isKeyDown(KeyCode::SPACE)) {
			jump(deltaTime);
		}

		if (playerRigidBody.groundEntity != max<Entity>()) {
			playerRigidBody.linearVelocity *= {1.0f, 0.0f, 1.0f};

			applyFriction(deltaTime);
		}

		if (playerRigidBody.groundEntity != max<Entity>()) {
			groundMove(deltaTime);
		} else {
			airMove(deltaTime);
		}

		playerRigidBody.linearVelocity += PLAYER_GRAVITY * deltaTime * 0.5f;
	}

	void Sample3DForwardLayer::jump(float32 deltaTime) {
		RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

		if (playerRigidBody.groundEntity == max<Entity>()) {
			return;
		}

		playerRigidBody.groundEntity = max<Entity>();

		playerRigidBody.linearVelocity += {0.0f, PLAYER_JUMP_FORCE, 0.0f};
		playerRigidBody.linearVelocity += PLAYER_GRAVITY * deltaTime * 0.5f;
	}

	void Sample3DForwardLayer::applyFriction(float32 deltaTime) {
		RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

		const float32 speed = length(playerRigidBody.linearVelocity);

		// If too slow, return.
		if (speed < 0.1f) {
			return;
		}

		// This makes that once at a slow speed (< 100) the friction will not get below 100 * f * Δt, thus slowing us down faster.
		const float32 control = max(speed, PLAYER_STOP_SPEED);
		const float32 drop = control * PLAYER_FRICTION * deltaTime;

		// Scale the velocity, then determine proportion of old speed we are using.
		const float32 newSpeed = max(0.0f, speed - drop) / speed;

		// Scale velocity according to proportion.
		playerRigidBody.linearVelocity *= newSpeed;
	}

	void Sample3DForwardLayer::groundAccelerate(vec3<float32> direction, float32 wishspeed, float32 acceleration, float32 deltaTime) {
		RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

		// See if we are changing direction a bit.
		const float32 speedTowardsDirection = dot(playerRigidBody.linearVelocity, direction);

		// Reduce wishspeed by the amount of veer.
		const float32 speedToAdd = wishspeed - speedTowardsDirection;

		if (speedToAdd <= 0.0f) {
			return;
		}

		const float32 accelerationSpeed = min(speedToAdd, wishspeed * acceleration * deltaTime);

		playerRigidBody.linearVelocity += direction * accelerationSpeed;
	}

	void Sample3DForwardLayer::airAccelerate(vec3<float32> direction, float32 wishspeed, float32 acceleration, float32 deltaTime) {
		RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

		// See if we are changing direction a bit.
		const float32 speedTowardsDirection = dot(playerRigidBody.linearVelocity, direction);

		// Reduce wishspeed by the amount of veer.
		const float32 speedToAdd = min(30.0f, wishspeed) - speedTowardsDirection;

		if (speedToAdd <= 0.0f) {
			return;
		}

		const float32 accelerationSpeed = min(speedToAdd, wishspeed * acceleration * deltaTime);

		playerRigidBody.linearVelocity += direction * accelerationSpeed;
	}

	void Sample3DForwardLayer::groundMove(float32 deltaTime) {
		RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

		float32 wishspeed = length(desiredVelocity);

		if (wishspeed == 0.0f) {
			return;
		}

		const vec3<float32> wishdir = normalized(desiredVelocity);

		if (wishspeed > PLAYER_MAX_SPEED) {
			wishspeed = PLAYER_MAX_SPEED;
		}

		playerRigidBody.linearVelocity *= {1.0f, 0.0f, 1.0f};

		groundAccelerate(wishdir, wishspeed, GROUND_ACCELERATE, deltaTime);

		playerRigidBody.linearVelocity *= {1.0f, 0.0f, 1.0f};

		if (lengthSquared(playerRigidBody.linearVelocity) < 1.0f) {
			playerRigidBody.linearVelocity = {0.0f, 0.0f, 0.0f};

			return;
		}

		playerRigidBody.groundEntity = max<Entity>();
	}

	void Sample3DForwardLayer::airMove(float32 deltaTime) {
		float32 wishspeed = length(desiredVelocity);

		if (wishspeed == 0.0f) {
			return;
		}

		const vec3<float32> wishdir = normalized(desiredVelocity);

		if (wishspeed > PLAYER_MAX_SPEED) {
			wishspeed = PLAYER_MAX_SPEED;
		}

		airAccelerate(wishdir, wishspeed, AIR_ACCELERATE, deltaTime);
	}

	void Sample3DForwardLayer::noclipMove(float32 deltaTime) {
		const Window& window = *getApplication()->getWindow();

		PerspectiveCamera3D& camera = cameraController.getCamera();

		const float32 forward = static_cast<float32>(window.isKeyDown(KeyCode::Z));
		const float32 back = static_cast<float32>(window.isKeyDown(KeyCode::S));
		const float32 right = static_cast<float32>(window.isKeyDown(KeyCode::D));
		const float32 left = static_cast<float32>(window.isKeyDown(KeyCode::Q));

		const vec3<float32> wishvel = camera.getForward() * (forward - back) + camera.getRight() * (right - left);

		if (lengthSquared(wishvel) == 0.0f) {
			return;
		}

		const vec3<float32> wishdir = normalized(wishvel) * MOVEMENT_SPEED * deltaTime;

		camera.position += wishdir;

		renderer.resetFrameIndex();
	}
}