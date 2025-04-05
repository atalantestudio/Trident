#pragma once

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline : public RenderPipeline {
		public:
			OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline();

		public:
			const OpenGLPipelineLayout& layout;

			GLuint vertexArray;
			const GLuint program;
			const GLenum primitiveTopology;
			const GLenum frontFace;
			const GLenum cullMode;
	};
}