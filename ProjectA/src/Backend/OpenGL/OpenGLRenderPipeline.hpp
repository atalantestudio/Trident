#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline : public RenderPipeline {
		public:
			OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline();

		public:
			GLuint vertexArray;
			const GLuint program;
			const GLenum primitiveTopology;
			const GLenum frontFace;
			const GLenum cullMode;
	};
}