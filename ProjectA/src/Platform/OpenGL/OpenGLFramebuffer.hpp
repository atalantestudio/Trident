#pragma once

#include "ProjectA/Renderer/Framebuffer.hpp"

namespace ProjectA {
	class OpenGLFramebuffer : public Framebuffer {
		private:
			uint32 id;
			uint32 colorAttachmentId;
			uint32 depthAttachmentId;

		public:
			OpenGLFramebuffer(const FramebufferDescriptor& descriptor);

			virtual ~OpenGLFramebuffer();

			virtual uint32 getColorAttachmentId() const override;
			virtual uint32 getDepthAttachmentId() const override;

			virtual void bind() override;
			virtual void unbind() override;

			virtual void resize(const vec2<float32>& size) override;

		private:
			void reset();
	};
}