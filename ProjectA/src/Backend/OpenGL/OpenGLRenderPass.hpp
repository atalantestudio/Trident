#pragma once

#include "Backend/OpenGL/OpenGLImage.hpp"

#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class OpenGLRenderPass : public RenderPass {
		public:
			OpenGLRenderPass(const RenderPassDescriptor& descriptor);

			ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const final override;

			ref<Image> createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const final override;

		private:
			static GLuint createScreenProgram();
	};
}