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
			chain<ColorAttachment> colorAttachments;
			DepthStencilAttachment depthStencilAttachment;

			GLuint screenProgram = 0;
	};
}