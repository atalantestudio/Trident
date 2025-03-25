#pragma once

#include "Engine/Renderer/Framebuffer.hpp"

// TODO: Revisit old API.
namespace ProjectA {
	class OpenGLFramebuffer : public Framebuffer {
		private:
			uint32 id;
			uint32 colorAttachmentId;
			uint32 depthAttachmentId;

		public:
			OpenGLFramebuffer(const FramebufferDescriptor& descriptor);

			~OpenGLFramebuffer();

			uint32 getColorAttachmentId() const override;
			uint32 getDepthAttachmentId() const override;

			void bind() override;
			void unbind() override;

			void resize(const vec2<float32>& size) override;

		private:
			void reset();
	};
}