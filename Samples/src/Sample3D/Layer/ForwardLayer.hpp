// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#define FORWARD_USE_NOCLIP_CAMERA
// #define FORWARD_ENABLE_PHYSICS
// #define FORWARD_TEST_CORNELL_BOX

#if !defined(FORWARD_USE_NOCLIP_CAMERA)
	#include "Engine/API/Camera/Controller/PlayerCameraController.hpp"
#else
	#include "Engine/API/Camera/Controller/NoClipCameraController.hpp"
#endif

#include "Engine/API/Event/EventDispatcher.hpp"
#include "Engine/API/Physics/3D/PhysicsSimulation3D.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class Sample3DForwardLayer : public Layer {
		private:
			#if defined(FORWARD_TEST_CORNELL_BOX)
				static constexpr float32 MOVEMENT_SPEED = 800.0f;
			#else
				static constexpr float32 MOVEMENT_SPEED = 120.0f;
			#endif

			static constexpr Entity PLAYER_ENTITY = 0;
			static constexpr float32 LOOK_SENSITIVITY = 0.075f;
			inline static const vec3<float32> PLAYER_CAMERA_OFFSET = {0.0f, 0.3f, 0.0f};

			// Values taken from HL1.
			// See https://github.com/valvesoftware/halflife.
			inline static const vec3<float32> PLAYER_GRAVITY = {0.0f, -800.0f, 0.0f};
			static constexpr float32 PLAYER_SPEED = 400.0f;
			static constexpr float32 PLAYER_MAX_SPEED = 270.0f;
			static constexpr float32 PLAYER_STOP_SPEED = 100.0f;
			static constexpr float32 PLAYER_JUMP_FORCE = 268.3f;

			static constexpr float32 GROUND_ACCELERATE = 10.0f;
			static constexpr float32 AIR_ACCELERATE = 10.0f;

			// See https://developer.valvesoftware.com/wiki/Sv_friction.
			static constexpr float32 PLAYER_FRICTION = 4.0f;

		public:
			~Sample3DForwardLayer() {
				delete scene;
				delete physicsSimulation;
			}

			void initialize() override {
				renderer.setLayer(this);
				renderer.build();
				renderer.postBuild();

				Application* application = getApplication();

				const view<char8> sceneAssetName = application->getPropertyFile()->getProperty<view<char8>>("AssetName");
				const ref<Scene3DAsset> sceneAsset = application->getAssetLibrary()->getAsset<Scene3DAsset>(sceneAssetName);

				// TODO(forward): Don't use alignment outside of the renderer.
				const uint64 alignment = renderer.getDevice()->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;

				scene = new Scene3D(sceneAsset, alignment);

				physicsSimulation = new PhysicsSimulation3D({
					.scene = scene,
					.deltaTime = getApplication()->getFixedDeltaTime(),
					#if !defined(FORWARD_USE_NOCLIP_CAMERA)
						.playerEntity = PLAYER_ENTITY,
					#else
						.playerEntity = max<Entity>(),
					#endif
				});

				renderer.setScene(scene);
				renderer.visualizationMode = VisualizationMode::DEFAULT;

				PerspectiveCamera3D& camera = cameraController.getCamera();
				camera.nearPlane = 0.01f;
				camera.farPlane = 2048.0f;
				camera.fieldOfView = radians(70.0_deg);
				camera.aspectRatio = getApplication()->getWindow()->getAspectRatio();

				#if defined(FORWARD_USE_NOCLIP_CAMERA)
					#if defined(FORWARD_TEST_CORNELL_BOX)
						camera.position = {-278.0f, 273.0f, -800.0f};
						camera.fieldOfView = radians(40.0_deg);
					#else
						// TODO(forward/camera): Expose the position.
						camera.position = {0.0f, 64.0f, 0.0f};
					#endif
				#endif

				cameraController.updateProjection();

				camera.updateView();
				camera.updateProjectionView();

				renderer.setCameraController<PerspectiveCamera3D>(&cameraController);
			}

			void encodeOneTimeCommands() override {
				renderer.encodeOneTimeCommands();
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					RigidBody3DComponent& playerRigidBody = scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY);
				#endif

				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					handleInputs();
				#endif

				#if defined(FORWARD_ENABLE_PHYSICS)
					physicsSimulation->tick();

					physicsSimulation->broadPhase();
					physicsSimulation->narrowPhase();
					physicsSimulation->applyForces();
				#endif

				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					playerMove(deltaTime);
				#else
					noclipMove(deltaTime);
				#endif

				#if defined(FORWARD_ENABLE_PHYSICS)
					physicsSimulation->updateContactManifolds();
					physicsSimulation->applyImpulses();

					// TODO(debug): Apply a rotational constraint to the player entity.
					// scene->ecs.getComponent<RigidBody3DComponent>(PLAYER_ENTITY).angularVelocity = {0.0f, 0.0f, 0.0f};

					physicsSimulation->integrate();
				#endif

				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					PerspectiveCamera3D& camera = cameraController.getCamera();
					camera.position = playerRigidBody.position + PLAYER_CAMERA_OFFSET;
				#endif

				cameraController.update(deltaTime);
			}

			void preRender() override {
				#if defined(FORWARD_ENABLE_PHYSICS)
					const ComponentPool<RigidBody3DComponent>& rigidBodyComponentPool = scene->ecs.getComponentPool<RigidBody3DComponent>();

					uint32 componentIndex = 0;

					while (componentIndex < rigidBodyComponentPool.componentCount) {
						const Entity entity = rigidBodyComponentPool.componentIndexToEntity[componentIndex];
						const RigidBody3DComponent& rigidBodyComponent = rigidBodyComponentPool.components[componentIndex];

						renderer.setInstanceWorldTransform(entity, rigidBodyComponent.world);
						renderer.setInstanceNormalTransform(entity, rigidBodyComponent.calculateNormal());

						componentIndex += 1;
					}

					if (renderer.visualizationMode & VisualizationMode::CONTACT_MANIFOLDS) {
						renderer.contactCount = 0;

						for (const std::pair<EntityPair, ContactPair3D>& pair : physicsSimulation->contactPairs) {
							if (pair.first.A != PLAYER_ENTITY && pair.first.B != PLAYER_ENTITY) {
								continue;
							}

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
				#endif

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

				/* if (pitch != 0.0f || yaw != 0.0f) {
					renderer.resetFrameIndex();
				} */

				return true;
			}

			bool onWindowResize(WindowResizeEvent& event) {
				cameraController.getCamera().aspectRatio = getApplication()->getWindow()->getAspectRatio();

				#if !defined(FORWARD_USE_NOCLIP_CAMERA)
					cameraController.getCamera().updateProjection();
				#else
					cameraController.updateProjection();
				#endif

				renderer.resize(event);

				return true;
			}

			void handleInputs();

			void playerMove(float32 deltaTime);

			void jump(float32 deltaTime);

			void applyFriction(float32 deltaTime);

			void groundAccelerate(
				/// The normalized direction in which the player wants to go.
				vec3<float32> direction,

				float32 wishspeed,

				/// The acceleration constant.
				float32 acceleration,

				float32 deltaTime
			);

			void airAccelerate(
				/// The normalized direction in which the player wants to go.
				vec3<float32> direction,

				float32 wishspeed,

				/// The acceleration constant.
				float32 acceleration,

				float32 deltaTime
			);

			void groundMove(float32 deltaTime);
			void airMove(float32 deltaTime);

			void noclipMove(float32 deltaTime);

		private:
			ForwardRenderer renderer;
			Scene3D* scene;
			PhysicsSimulation3D* physicsSimulation;

			#if !defined(FORWARD_USE_NOCLIP_CAMERA)
				PlayerCameraController cameraController;
			#else
				NoClipCameraController cameraController;
			#endif

			vec3<float32> desiredVelocity = {0.0f, 0.0f, 0.0f};
	};
}