// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/3D/PerspectiveCamera3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	class BaseSceneRenderer : public SceneRenderer<Scene3D> {
		public:
			struct Camera {
				mat4x4<float32> inverseProjection;
				mat4x4<float32> inverseView;
				vec3<float32> position;
				Extent2D viewport;
				uint32 frameIndex;
			};

		public:
			void postSetCameraController() override {}

			void resize(WindowResizeEvent& event) override {
				initialize();
				updateCamera();
			}

			virtual void initialize() = 0;

			void updateCamera() {
				const PerspectiveCamera3D& camera = getCameraController<PerspectiveCamera3D>().getCamera();

				cameraMappedRange->inverseProjection = inverse(camera.getProjection());
				cameraMappedRange->inverseView = inverse(camera.getView());
				cameraMappedRange->position = camera.position;
				cameraMappedRange->viewport = getSwapchain()->getExtent();
			}

		protected:
			ref<Buffer<Camera>> createCameraBuffer() const;
			ref<Buffer<uint8>> createStaticBuffer() const;
			ref<Buffer<uint8>> createStagingStaticBuffer() const;

			void copyBuffers() const {
				const ref<Buffer<uint8>> staticStagingBuffer = createStagingStaticBuffer();

				const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::TRANSFER);
				const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
					.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Copy buffers",
					#endif
				});

				commandEncoder->begin();

				copyStaticBuffer(commandEncoder, staticStagingBuffer);

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				queue->submit(commandBuffer);
				queue->waitIdle();
			}

			void copyStaticBuffer(ref<CommandEncoder> commandEncoder, ref<Buffer<uint8>> staticStagingBuffer) const;

		protected:
			/// Contains the view/camera data.
			ref<Buffer<Camera>> cameraBuffer;

			/// Contains the static scene data.
			ref<Buffer<uint8>> staticBuffer;

			Camera* cameraMappedRange = nullptr;
	};
}