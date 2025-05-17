#pragma once

#include "Backend/OpenGL/OpenGLImage.hpp"

#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	class OpenGLRenderPass : public RenderPass {
		public:
			OpenGLRenderPass(const RenderPassDescriptor& descriptor, const Window& window);

			~OpenGLRenderPass();

		public:
			LoadOperation loadOperation;
			GLuint multisampleFramebuffer = 0;
			GLuint multisampleIntermediateFramebuffer = 0;
			GLuint multisampleProgram = 0;
			scope<OpenGLImage> multisampleImage;
			scope<OpenGLImage> screenImage;
	};
}