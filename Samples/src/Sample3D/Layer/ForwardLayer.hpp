// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#define FORWARD_USE_NOCLIP_CAMERA

#if !defined(FORWARD_USE_NOCLIP_CAMERA)
	#include "Engine/API/Camera/Controller/PlayerCameraController.hpp"
#else
	#include "Engine/API/Camera/Controller/FlyCameraController.hpp"
#endif

#include "Engine/API/Physics/3D/PhysicsSimulation3D.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"

#include "../Scene/FlatColor.hpp"

namespace Trident {
	class Sample3DForwardLayer : public Layer {
		private:
			static constexpr float32 MOVEMENT_SPEED = 2.0f;
			static constexpr Entity PLAYER_ENTITY = 1;

			static constexpr float32 LOOK_SENSITIVITY = 0.075f;

			inline static const vec3<float32> PLAYER_CAMERA_OFFSET = {0.0f, 0.3f, 0.0f};

			// Values taken from HL1.
			// See https://github.com/valvesoftware/halflife.
			inline static const vec3<float32> PLAYER_GRAVITY = {0.0f, -800.0f, 0.0f};
			static constexpr float32 PLAYER_SPEED = 400.0f;
			static constexpr float32 PLAYER_MAX_SPEED = 270.0f;
			static constexpr float32 PLAYER_STOP_SPEED = 100.0f;

			static constexpr float32 GROUND_ACCELERATE = 10.0f;
			static constexpr float32 AIR_ACCELERATE = 10.0f;

			// See https://developer.valvesoftware.com/wiki/Sv_friction.
			static constexpr float32 PLAYER_FRICTION = 4.0f;

		public:
			void build() override {
				renderer.setLayer(this);
				renderer.build();

				scene = createScope<Scene3D>(FlatColor::initializeScene(getApplication(), renderer.getGraphicsApi(), renderer.getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment));

				physicsSimulation = createScope<PhysicsSimulation3D>(Descriptor<PhysicsSimulation3D>{
					.scene = scene.get(),
					.deltaTime = getApplication()->getFixedDeltaTime(),
				});

				renderer.setScene(scene.get());

				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					PerspectiveCamera3D& camera = cameraController.getCamera();
					camera.distance = 256.0_m;
					camera.fieldOfView = radians(60.0_deg);
					camera.aspectRatio = getApplication()->getWindow()->getAspectRatio();
					camera.nearPlane = 0.01_m;
					camera.farPlane = 4096.0_m;
					camera.updateProjection();
					camera.updateView();
					camera.updateProjectionView();
				#else
					cameraController.getCamera().position = {0.0_m, 0.9_m, -2.0_m};
					cameraController.getCamera().aspectRatio = getApplication()->getWindow()->getAspectRatio();
					cameraController.updateProjection();
				#endif

				renderer.setCameraController<PerspectiveCamera3D>(&cameraController);
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					handleInputs();
					handlePlayerMovement(deltaTime);
				#else
					noclipMove(deltaTime);
				#endif

				physicsSimulation->tick();

				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					const RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

					PerspectiveCamera3D& camera = cameraController.getCamera();
					camera.position = playerRigidBody.position + PLAYER_CAMERA_OFFSET;
				#endif

				cameraController.update(deltaTime);
			}

			void preRender() override {
				const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

				uint32 componentIndex = 0;

				while (componentIndex < rigidBodyComponentPool.componentCount) {
					const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
					const RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

					renderer.setWorld(entity, rigidBodyComponent.world);

					componentIndex += 1;
				}

				// Visualize contact manifolds.
				{
					renderer.contactCount = 0;

					for (const std::pair<EntityPair, ContactPair3D>& pair : physicsSimulation->contactPairs) {
						uint8 contactIndex = 0;

						while (contactIndex < pair.second.manifold.contactCount) {
							renderer.debugContactRange[renderer.contactCount] = svec3<float32>(pair.second.manifold.contacts[contactIndex].position);
							renderer.contactCount += 1;

							contactIndex += 1;
						}

						renderer.debugContactRange[renderer.contactCount] = svec3<float32>(pair.second.manifold.contacts[0].position);
						renderer.contactCount += 1;

						break;
					}
				}

				renderer.updateCamera();
			}

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);
				eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));
				eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
				eventDispatcher.dispatch<MouseMoveEvent>(BIND(onMouseMove));
				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));

				renderer.onEvent(event);
			}

		private:
			bool onKeyDown(KeyDownEvent& event) {
				const Window* window = getApplication()->getWindow();

				if (event.getKeyCode() == KeyCode::ESCAPE && window->isPointerLocked()) {
					window->unlockPointer();
				}

				return true;
			}

			bool onMouseDown(MouseDownEvent& event) {
				const Window* window = getApplication()->getWindow();

				if (!window->isPointerLocked()) {
					window->lockPointer();
				}

				return true;
			}

			bool onMouseMove(MouseMoveEvent& event) {
				if (!getApplication()->getWindow()->isPointerLocked()) {
					return true;
				}

				const float32 pitch = event.getY() * LOOK_SENSITIVITY;
				const float32 yaw = event.getX() * LOOK_SENSITIVITY;

				cameraController.accumulateAngularMotion(radians(vec3<float32>{pitch, yaw, 0.0_deg}));

				return true;
			}

			bool onWindowResize(WindowResizeEvent& event) {
				cameraController.getCamera().aspectRatio = getApplication()->getWindow()->getAspectRatio();
				cameraController.getCamera().updateProjection();

				renderer.resize(event);

				return true;
			}

			void noclipMove(float32 deltaTime) {
				const Window& window = *getApplication()->getWindow();

				if (!window.isPointerLocked()) {
					return;
				}

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
			}

			void handleInputs() {
				forwardmove = 0;
				sidemove = 0;

				const Window& window = *getApplication()->getWindow();

				if (!window.isPointerLocked()) {
					return;
				}

				const float32 forward = static_cast<float32>(window.isKeyDown(KeyCode::Z));
				const float32 back = static_cast<float32>(window.isKeyDown(KeyCode::S));
				const float32 right = static_cast<float32>(window.isKeyDown(KeyCode::D));
				const float32 left = static_cast<float32>(window.isKeyDown(KeyCode::Q));

				forwardmove += PLAYER_SPEED * forward;
				forwardmove -= PLAYER_SPEED * back;
				sidemove += PLAYER_SPEED * right;
				sidemove -= PLAYER_SPEED * left;
			}

			// wishdir = The normalized direction that the player has requested to move (taking into account the movement keys and look direction)
			// accel = The server-defined player acceleration value
			void accelerate(vec3<float32> wishdir, float32 wishspeed, float32 accel, float32 deltaTime) {
				RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

				// See if we are changing direction a bit.
				float32 currentspeed = dot(playerRigidBody.linearVelocity, wishdir);

				// Reduce wishspeed by the amount of veer.
				float32 addspeed = wishspeed - currentspeed;

				// If not going to add any speed, done.
				if (addspeed <= 0.0f) {
					return;
				}

				// Determine amount of acceleration.
				float32 accelspeed = wishspeed * accel * deltaTime; // friction = 1?

				// Cap at addspeed.
				if (accelspeed > addspeed) {
					accelspeed = addspeed;
				}

				playerRigidBody.linearVelocity += wishdir * accelspeed;
			}

			void walkMove(float32 deltaTime) {
				RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

				const PerspectiveCamera3D& camera = cameraController.getCamera();
				const vec3<float32> forward = normalized(camera.getForward() * vec3<float32>{1.0f, 0.0f, 1.0f});
				const vec3<float32> right = normalized(camera.getRight() * vec3<float32>{1.0f, 0.0f, 1.0f});

				vec3<float32> wishvel = {
					forward[0] * forwardmove + right[0] * sidemove,
					0.0f,
					forward[2] * forwardmove + right[2] * sidemove,
				};
				float32 wishspeed = length(wishvel);

				if (wishspeed == 0.0f) {
					return;
				}

				const vec3<float32> wishdir = normalized(wishvel);

				// Clamp to max speed
				if (wishspeed > PLAYER_MAX_SPEED) {
					wishvel *= PLAYER_MAX_SPEED / wishspeed;
					wishspeed = PLAYER_MAX_SPEED;
				}

				playerRigidBody.linearVelocity *= {1.0f, 0.0f, 1.0f};

				accelerate(wishdir, wishspeed, GROUND_ACCELERATE, deltaTime);

				playerRigidBody.linearVelocity *= {1.0f, 0.0f, 1.0f};

				const float32 speed = length(playerRigidBody.linearVelocity);

				if (speed < 1.0f) {
					playerRigidBody.linearVelocity = {0.0f, 0.0f, 0.0f};

					return;
				}

				playerRigidBody.groundEntity = max<Entity>();
			}

			void applyVelocity(float32 deltaTime) {
				// TODO: Move into PhysicsSimulation3D?
				RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);
				playerRigidBody.position += playerRigidBody.linearVelocity * deltaTime;
				playerRigidBody.updateWorld({0.0f, 0.0f, 0.0f});
			}

			void handlePlayerMovement(float32 deltaTime) {
				static const float32 JUMP_FORCE = sqrt(2.0f * 800.0f * 45.0f);

				const Window& window = *getApplication()->getWindow();

				RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);

				// Add half the gravity.
				playerRigidBody.linearVelocity += PLAYER_GRAVITY * deltaTime * 0.5f;

				// Handle jumps.
				if (window.isKeyDown(KeyCode::SPACE)) {
					if (playerRigidBody.groundEntity != max<Entity>()) {
						// If on ground, allow jumps.
						playerRigidBody.linearVelocity += {0.0f, JUMP_FORCE, 0.0f};

						playerRigidBody.groundEntity = max<Entity>();
					}
				}

				// Apply friction.
				/* if (playerRigidBody.groundEntity != max<Entity>()) {
					// Reset Y before applying friction
					player.getVelocity()[1] = 0;

					applyFriction(deltaTime);
				} */

				// Walk.
				if (playerRigidBody.groundEntity != max<Entity>()) {
					// This nullifies the player's ground mesh.
					walkMove(deltaTime);
				} else {
					// airMove(deltaTime);
				}

				// Add half the gravity.
				playerRigidBody.linearVelocity += PLAYER_GRAVITY * deltaTime * 0.5f;

				if (playerRigidBody.groundEntity == max<Entity>()) {
					// Apply velocity.
					applyVelocity(deltaTime);
				} else {
					// If we are on ground, no downward velocity.
					playerRigidBody.linearVelocity *= {1.0f, 0.0f, 1.0f};
				}
			}

		private:
			ForwardRenderer renderer;
			scope<Scene3D> scene;
			scope<PhysicsSimulation3D> physicsSimulation;

			#if !defined(FORWARD_USE_NOCLIP_CAMERA)
				PlayerCameraController cameraController;
			#else
				FlyCameraController cameraController;
			#endif

			float32 forwardmove = 0.0f;
			float32 sidemove = 0.0f;
	};
}