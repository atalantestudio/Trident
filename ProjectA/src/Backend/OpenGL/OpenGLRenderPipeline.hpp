#pragma once

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline : public RenderPipeline {
		public:
			OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline();

		public:
			const OpenGLPipelineLayout& layout;

			#ifdef BUILD_MODE_DEBUG
				const DynamicState dynamicState;
			#endif

			GLuint vertexArray;
			const GLuint program;
			const GLenum primitiveTopology;
			const GLenum frontFace;
			const GLenum cullMode;
			const GLenum polygonModeFace;
			const GLenum polygonMode;
	};
}