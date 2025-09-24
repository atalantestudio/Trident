#pragma once

#include "Engine/Camera/3D/PerspectiveCamera3D.hpp"
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

				const ref<BoxGeometry3D> boxGeometry = createRef<BoxGeometry3D>(vec3<float32>(1.0_m, 1.0_m, 1.0_m));

				ref<ColorMaterial> boxMaterial = createRef<ColorMaterial>();
				boxMaterial->color = {1.0f, 0.2f, 0.0f, 1.0f};

				Entity3DDescriptor boxDescriptor{};
				boxDescriptor.position = {0.0_m, 0.0_m, 1.0_m};
				boxDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
				boxDescriptor.geometry = boxGeometry;
				boxDescriptor.material = boxMaterial;

				scene.createEntity(boxDescriptor);

				renderer.setScene(&scene);

				camera.updateProjection(FIELD_OF_VIEW, getApplication()->getWindow()->getAspectRatio(), NEAR_PLANE, FAR_PLANE);

				renderer.setCamera(&camera);
				renderer.updateCamera();
			}

			void update(float32 deltaTime, float32 inverseDeltaTime) override {
				static const vec3<float32> rotationAxis = {0.0f, 1.0f, 0.0f};

				RigidBody3D& boxRigidBody = scene.getRigidBody(scene.getLastEntity());
				boxRigidBody.rotation += rotationAxis * 1_rad * deltaTime;

				boxRigidBody.updateTransform();

				renderer.updateTransform(scene.getLastEntity());
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
			PerspectiveCamera3D camera;
	};
}