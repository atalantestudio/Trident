#pragma once

#include <glad/glad.h>

#include "ProjectA/Renderer/IndexBuffer.hpp"
#include "ProjectA/Renderer/RenderPipeline.hpp"
#include "ProjectA/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline : public RenderPipeline {
		public:
			GLuint vertexArrayObject;
			const GLuint programObject;
			const GLenum primitiveTopology;
			std::vector<ref<VertexBuffer>> vertexBuffers;
			ref<IndexBuffer> indexBuffer;

		public:
			OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline();

			// TODO: Remove
			virtual void bind() const override;
			virtual void unbind() const override;

			virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const override;

			virtual const ref<IndexBuffer>& getIndexBuffer() const override;

			virtual void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) override;

			virtual void setIndexBuffer(const ref<IndexBuffer>& indexBuffer) override;

			virtual void setUniform(const std::string& name, int32 value) const override;
			virtual void setUniform(const std::string& name, const int32* value, uint32 count) const override;
			virtual void setUniform(const std::string& name, float32 value) const override;
			virtual void setUniform(const std::string& name, const vec2<float32>& value) const override;
			virtual void setUniform(const std::string& name, const vec3<float32>& value) const override;
			virtual void setUniform(const std::string& name, const vec4<float32>& value) const override;
			virtual void setUniform(const std::string& name, const mat4x4<float32>& value) const override;

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