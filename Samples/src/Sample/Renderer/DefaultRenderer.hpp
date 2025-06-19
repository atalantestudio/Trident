#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DefaultRenderer : public Renderer {
		public:
			void build() override {
				FramebufferDescriptor framebufferDescriptor;
				framebufferDescriptor.renderPass = nullptr;
				framebufferDescriptor.attachmentImageViews = {};
				framebufferDescriptor.extent.width = 1280;
				framebufferDescriptor.extent.height = 720;
				framebufferDescriptor.layerCount = 1;

				ref<Framebuffer> framebuffer = createFramebuffer(framebufferDescriptor);

				BREAKPOINT();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BREAKPOINT();
			}

			void resize(WindowResizeEvent& event) override {
				BREAKPOINT();
			}
	};
}