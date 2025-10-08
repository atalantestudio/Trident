#pragma once

#include "Engine/API/Camera/Controller/FlyCameraController.hpp"
#include "Engine/Filesystem/Asset/FBXToGeometry.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/3D/VisibilityRenderer.hpp"

namespace ProjectA {
	class Sample3DLayer : public Layer {
		private:
			static constexpr float32 SENSITIVITY = 0.075f;

		public:
			explicit Sample3DLayer() {}

			void build() override {
				renderer.setLayer(this);
				renderer.build();

				static constexpr char8* lemonPath = "Asset/Model/lemon/lemon.fbx";
				static constexpr char8* stoolPath = "Asset/Model/folding_wooden_stool/folding_wooden_stool.fbx";

				const sequence<uint8> lemonData = Filesystem::openBuffer(lemonPath);
				const File<FileFormat::FBX> lemonFile = Filesystem::decode<FileFormat::FBX>(lemonData);

				const sequence<uint8> stoolData = Filesystem::openBuffer(stoolPath);
				const File<FileFormat::FBX> stoolFile = Filesystem::decode<FileFormat::FBX>(stoolData);

				const uint32 lemonGeometryIndex = scene.registerGeometry(FBXToGeometry3D(lemonFile));
				const uint32 stoolGeometryIndex = scene.registerGeometry(FBXToGeometry3D(stoolFile));

				InstanceDescriptor lemon1InstanceDescriptor{};
				lemon1InstanceDescriptor.geometryIndex = lemonGeometryIndex;
				lemon1InstanceDescriptor.position = {-0.1_m, 0.0_m, 0.3_m};
				lemon1InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};

				InstanceDescriptor lemon2InstanceDescriptor{};
				lemon2InstanceDescriptor.geometryIndex = lemonGeometryIndex;
				lemon2InstanceDescriptor.position = {0.0_m, 0.0_m, 0.3_m};
				lemon2InstanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};

				InstanceDescriptor stoolInstanceDescriptor{};
				stoolInstanceDescriptor.geometryIndex = stoolGeometryIndex;
				stoolInstanceDescriptor.position = {0.5_m, -0.225_m, 1.0_m};
				stoolInstanceDescriptor.rotation = {pi<float32>() * 0.5f, 0.3_rad, 0.0_rad};

				scene.registerInstance(lemon1InstanceDescriptor);
				scene.registerInstance(lemon2InstanceDescriptor);
				scene.registerInstance(stoolInstanceDescriptor);

				/* static constexpr char8* shelfPath = "Asset/Model/shelf/shelf.fbx";
				static constexpr char8* woodenTable02Path = "Asset/Model/wooden_table_02/wooden_table_02.fbx";

				const sequence<uint8> shelfData = Filesystem::openBuffer(shelfPath);
				const File<FileFormat::FBX> shelfFile = Filesystem::decode<FileFormat::FBX>(shelfData);

				const sequence<uint8> woodenTable02Data = Filesystem::openBuffer(woodenTable02Path);
				const File<FileFormat::FBX> woodenTable02File = Filesystem::decode<FileFormat::FBX>(woodenTable02Data);

				const uint32 woodenTable02GeometryIndex = scene.registerGeometry(FBXToGeometry3D(woodenTable02File));
				const uint32 shelfGeometryIndex = scene.registerGeometry(FBXToGeometry3D(shelfFile));

				InstanceDescriptor shelf1InstanceDescriptor{};
				shelf1InstanceDescriptor.geometryIndex = shelfGeometryIndex;
				shelf1InstanceDescriptor.position = {-2.0_m, -1.0_m, 3.0_m};
				shelf1InstanceDescriptor.rotation = {pi<float32>() * 0.5f, 0.0_rad, 0.0_rad};

				InstanceDescriptor woodenTable02InstanceDescriptor{};
				woodenTable02InstanceDescriptor.geometryIndex = woodenTable02GeometryIndex;
				woodenTable02InstanceDescriptor.position = {0.0_m, -1.0_m, 3.0_m};
				woodenTable02InstanceDescriptor.rotation = {pi<float32>() * 0.5f, pi<float32>(), 0.0_rad};

				InstanceDescriptor shelf2InstanceDescriptor{};
				shelf2InstanceDescriptor.geometryIndex = shelfGeometryIndex;
				shelf2InstanceDescriptor.position = {2.0_m, -1.0_m, 3.0_m};
				shelf2InstanceDescriptor.rotation = {pi<float32>() * 0.5f, pi<float32>(), 0.0_rad};

				scene.registerInstance(shelf1InstanceDescriptor);
				scene.registerInstance(woodenTable02InstanceDescriptor);
				scene.registerInstance(shelf2InstanceDescriptor); */

				clusteredScene = clusterizeScene(scene);

				renderer.setScene(&clusteredScene);

				cameraController.setAspectRatio(getApplication()->getWindow()->getAspectRatio());
				cameraController.updateProjection();

				renderer.setCameraController<PerspectiveCamera>(&cameraController);
				renderer.updateCameraProjectionView();
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				static constexpr uint32 instanceIndex = 0;
				static const vec3<float32> rotationAxis = {0.0f, 1.0f, 0.0f};

				RigidBody3D& rigidBody = scene.getInstance(instanceIndex);
				rigidBody.rotation += rotationAxis * 1_rad * deltaTime;

				rigidBody.updateTransform();

				renderer.updateInstanceTransform(instanceIndex, rigidBody.transform);

				// cameraController.update(deltaTime);

				// renderer.updateCameraProjectionView();
			}

			void preRender() override {
				// TODO: Move to update().
				cameraController.update(0.0f);
				renderer.updateCameraProjectionView();
			}

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);
				// eventDispatcher.dispatch<KeyDownEvent>(BIND(onKeyDown));
				eventDispatcher.dispatch<MouseDownEvent>(BIND(onMouseDown));
				eventDispatcher.dispatch<MouseMoveEvent>(BIND(onMouseMove));
				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));

				renderer.onEvent(event);
			}

		private:
			/* bool onKeyDown(KeyDownEvent& event) {
				switch (event.getKeyCode()) {
					case KeyCode::P: {
						const Window* window = getApplication()->getWindow();

						if (!window->isPointerLocked()) {
							window->lockPointer();
						}
						else {
							window->unlockPointer();
						}

						break;
					}
					default:
						break;
				}

				return true;
			} */

			bool onMouseDown(MouseDownEvent& event) {
				const Window* window = getApplication()->getWindow();

				if (!window->isPointerLocked()) {
					window->lockPointer();
				}
				else {
					window->unlockPointer();
				}

				return true;
			}

			bool onMouseMove(MouseMoveEvent& event) {
				if (!getApplication()->getWindow()->isPointerLocked()) {
					return true;
				}

				// TODO: Correct initial delta values when first locking the pointer.
				const float32 pitch = static_cast<float32>(event.getY());
				const float32 yaw = static_cast<float32>(event.getX());
				const vec3<float32> rotation = {pitch, yaw, 0.0_deg};

				cameraController.addAngularMotion(radians(rotation));

				return true;
			}

			bool onWindowResize(WindowResizeEvent& event) {
				cameraController.setAspectRatio(getApplication()->getWindow()->getAspectRatio());
				cameraController.updateProjection();

				return true;
			}

		private:
			VisibilityRenderer renderer;
			Scene3D scene;
			ClusteredScene clusteredScene;
			FlyCameraController cameraController;
	};
}