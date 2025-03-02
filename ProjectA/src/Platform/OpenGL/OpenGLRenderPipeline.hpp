#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline : public RenderPipeline {
		public:
			GLuint vertexArrayObject;
			const GLuint programObject;
			const GLenum primitiveTopology;
			//std::vector<ref<VertexBuffer>> vertexBuffers;
			//ref<IndexBuffer> indexBuffer;

		public:
			OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline();

		private:
			static constexpr GLenum primitiveTopologies[5] = {
				GL_POINTS,
				GL_LINES,
				GL_LINE_STRIP,
				GL_TRIANGLES,
				GL_TRIANGLE_STRIP,
			};

		private:
			GLint getUniformLocation(const std::string& name) const;
	};
}