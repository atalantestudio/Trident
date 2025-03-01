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

			// TODO: Remove
			void bind() const override;
			void unbind() const override;

			//const std::vector<ref<VertexBuffer>>& getVertexBuffers() const override;

			//const ref<IndexBuffer>& getIndexBuffer() const override;

			//void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) override;

			//void setIndexBuffer(const ref<IndexBuffer>& indexBuffer) override;

			void setUniform(const std::string& name, int32 value) const override;
			void setUniform(const std::string& name, const int32* value, uint32 count) const override;
			void setUniform(const std::string& name, float32 value) const override;
			void setUniform(const std::string& name, const vec2<float32>& value) const override;
			void setUniform(const std::string& name, const vec3<float32>& value) const override;
			void setUniform(const std::string& name, const vec4<float32>& value) const override;
			void setUniform(const std::string& name, const mat4x4<float32>& value) const override;

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