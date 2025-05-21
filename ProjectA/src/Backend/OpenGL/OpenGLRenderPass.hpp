#pragma once

#include "Backend/OpenGL/OpenGLImage.hpp"

#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class OpenGLRenderPass : public RenderPass {
		public:
			OpenGLRenderPass(const RenderPassDescriptor& descriptor);

		private:
			static GLuint createScreenProgram();

		public:
			uint32 colorAttachmentCount;

			// This includes color/resolve attachments and the depth stencil attachment.
			chain<Attachment> attachments;

			GLuint screenProgram = 0;
	};
}