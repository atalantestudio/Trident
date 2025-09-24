#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class Renderer3D : public Renderer {
		public:
			virtual void setScene(const Scene3D* scene) {
				this->scene = scene;
			}

			void setCamera(const Camera3D* camera) {
				this->camera = camera;
			}

		protected:
			void recreateFramebuffers() {
				const ref<Swapchain>& swapchain = getSwapchain();

				if (framebuffers.count > 0) {
					for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
						framebuffers[framebufferIndex].~shared_ptr();
					}
				}

				framebuffers = sequence<ref<Framebuffer>>(swapchain->getImageCount());

				FramebufferDescriptor framebufferDescriptor{};
				framebufferDescriptor.renderPass = renderPass;
				framebufferDescriptor.imageViews = sequence<ref<ImageView>>(1);
				framebufferDescriptor.extent = swapchain->getExtent();
				framebufferDescriptor.layerCount = 1;

				for (uint32 framebufferIndex = 0; framebufferIndex < swapchain->getImageCount(); framebufferIndex += 1) {
					framebufferDescriptor.imageViews[0] = swapchain->getImageView(framebufferIndex);

					framebuffers[framebufferIndex] = createFramebuffer(framebufferDescriptor);
				}
			}

		protected:
			ref<Subpass> subpass;
			ref<RenderPass> renderPass;
			sequence<ref<Framebuffer>> framebuffers;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			const Scene3D* scene = nullptr;
			const Camera3D* camera = nullptr;
	};
}