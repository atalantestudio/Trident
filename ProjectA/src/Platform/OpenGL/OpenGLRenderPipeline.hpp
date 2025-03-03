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

		private:
			static constexpr GLenum primitiveTopologies[] = {
				GL_POINTS,
				GL_LINES,
				GL_LINE_STRIP,
				GL_TRIANGLES,
				GL_TRIANGLE_STRIP,
			};
	};
}