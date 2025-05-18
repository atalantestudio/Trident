#pragma once

#include "Backend/OpenGL/OpenGLImage.hpp"

#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	class OpenGLRenderPass : public RenderPass {
		public:
			OpenGLRenderPass(const RenderPassDescriptor& descriptor);

			~OpenGLRenderPass();

			void recreateMultisampleImages(uint32 extentWidth, uint32 extentHeight);

		public:
			LoadOperation loadOperation;

			GLuint multisampleFramebuffer = 0;
			GLuint multisampleIntermediateFramebuffer = 0;
			GLuint multisampleProgram = 0;
			scope<OpenGLImage> multisampleImage;
			scope<OpenGLImage> screenImage;

		private:
			uint8 sampleCount;
	};
}