#pragma once

#include "Engine/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/Filesystem/Asset/FBXToGeometry.hpp"
#include "Engine/Layer/Layer.hpp"
#include "Engine/Renderer/3D/ForwardRenderer3D.hpp"

namespace ProjectA {
	class Sample3DLayer : public Layer {
		public:
			explicit Sample3DLayer() :
				camera(mat4x4<float32>::identity())
			{}

			void build() override {
				renderer.setLayer(this);
				renderer.build();

				static constexpr char8* fbxPath = "Asset/Model/folding_wooden_stool/folding_wooden_stool.fbx";

				const sequence<uint8> fbxData = Filesystem::openBuffer(fbxPath);
				const File<FileFormat::FBX> fbxFile = Filesystem::decode<FileFormat::FBX>(fbxData);

				Entity3DDescriptor entityDescriptor{};
				entityDescriptor.position = {0.0_m, -0.225_m, 0.85_m};
				entityDescriptor.rotation = {pi<float32>() * 0.5f, 0.3_rad, 0.0_rad};
				entityDescriptor.geometry = FBXToGeometry3D(fbxFile);
				entityDescriptor.material = createRef<ColorMaterial>();

				scene.createEntity(entityDescriptor);

				// clusteredScene = clusterizeScene(scene);

				// renderer.setScene(&clusteredScene);
				renderer.setScene(&scene);

				camera.updateProjection(FIELD_OF_VIEW, getApplication()->getWindow()->getAspectRatio(), NEAR_PLANE, FAR_PLANE);

				renderer.setCamera(&camera);
				renderer.updateCamera();
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				static const vec3<float32> rotationAxis = {0.0f, 1.0f, 0.0f};

				const Entity entity = scene.getLastEntity();

				RigidBody3D& rigidBody = scene.getRigidBody(entity);
				rigidBody.rotation += rotationAxis * 1_rad * deltaTime;

				rigidBody.updateTransform();

				// renderer.updateInstanceTransform(entity, rigidBody.transform);
				renderer.updateTransform(entity);
			}

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);
				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
			}

		private:
			bool onWindowResize(WindowResizeEvent& event) {
				camera.updateProjection(FIELD_OF_VIEW, getApplication()->getWindow()->getAspectRatio(), NEAR_PLANE, FAR_PLANE);

				renderer.resize(event);

				return true;
			}

		private:
			static constexpr float32 FIELD_OF_VIEW = radians(60.0_deg);
			static constexpr float32 NEAR_PLANE = 0.0_m;
			static constexpr float32 FAR_PLANE = 100.0_m;

		private:
			ForwardRenderer3D renderer;
			Scene3D scene;
			// ClusteredScene clusteredScene;
			PerspectiveCamera3D camera;
	};
}