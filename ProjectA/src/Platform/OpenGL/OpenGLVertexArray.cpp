#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLVertexArray.hpp"

namespace ProjectA {
	// TODO: Replace with typename template
	// Use primitive types and math library types
	static GLenum getShaderType(ShaderType type) {
		switch (type) {
			case ShaderType::Bool:
				return GL_BOOL;
			case ShaderType::Int:
				return GL_INT;
			case ShaderType::Int2:
				return GL_INT;
			case ShaderType::Int3:
				return GL_INT;
			case ShaderType::Int4:
				return GL_INT;
			case ShaderType::Float:
				return GL_FLOAT;
			case ShaderType::Float2:
				return GL_FLOAT;
			case ShaderType::Float3:
				return GL_FLOAT;
			case ShaderType::Float4:
				return GL_FLOAT;
			case ShaderType::Mat3:
				return GL_FLOAT;
			case ShaderType::Mat4:
				return GL_FLOAT;
		}

		ENGINE_ASSERT(false, "Unknown ShaderType");

		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		PROFILE_SCOPE(__FUNCSIG__);

		glCreateVertexArrays(1, &id);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		PROFILE_SCOPE(__FUNCSIG__);

		glDeleteVertexArrays(1, &id);
	}

	void OpenGLVertexArray::bind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindVertexArray(id);
	}

	void OpenGLVertexArray::unbind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindVertexArray(0);
	}

	const std::vector<ref<VertexBuffer>>& OpenGLVertexArray::getVertexBuffers() const {
		return vertexBuffers;
	}

	const ref<IndexBuffer>& OpenGLVertexArray::getIndexBuffer() const {
		return indexBuffer;
	}

	void OpenGLVertexArray::addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) {
		PROFILE_SCOPE(__FUNCSIG__);

		ENGINE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has no layout");

		bind();
		vertexBuffer->bind();

		const auto& layout = vertexBuffer->getLayout();

		GLuint attributeIndex = 0;

		for (const auto& element : layout) {
			switch (element.type) {
				case ShaderType::Float:
				case ShaderType::Float2:
				case ShaderType::Float3:
				case ShaderType::Float4:
					glEnableVertexAttribArray(attributeIndex);
					glVertexAttribPointer(
						attributeIndex,
						getShaderTypeComponentCount(element.type),
						getShaderType(element.type),
						element.normalized ? GL_TRUE : GL_FALSE,
						layout.getStride(),
						(const void*) element.offset
					);

					attributeIndex++;

					break;
				case ShaderType::Bool:
				case ShaderType::Int:
				case ShaderType::Int2:
				case ShaderType::Int3:
				case ShaderType::Int4:
					glEnableVertexAttribArray(attributeIndex);
					glVertexAttribIPointer(
						attributeIndex,
						getShaderTypeComponentCount(element.type),
						getShaderType(element.type),
						layout.getStride(),
						(const void*) element.offset
					);

					attributeIndex++;

					break;
				case ShaderType::Mat3:
				case ShaderType::Mat4:
					const uint8 count = getShaderTypeComponentCount(element.type);
					const GLenum shaderType = getShaderType(element.type);
					const GLboolean normalized = element.normalized ? GL_TRUE : GL_FALSE;
					const uint32 stride = layout.getStride();

					for (uint8 i = 0; i < count; i++) {
						glEnableVertexAttribArray(attributeIndex);
						glVertexAttribPointer(
							attributeIndex,
							count,
							shaderType,
							normalized,
							stride,
							(const void*) (sizeof(float32) * count * i)
						);
						glVertexAttribDivisor(attributeIndex, 1);

						attributeIndex++;
					}

					break;
			}
		}

		vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const ref<IndexBuffer>& indexBuffer) {
		PROFILE_SCOPE(__FUNCSIG__);

		bind();
		indexBuffer->bind();

		this->indexBuffer = indexBuffer;
	}
}