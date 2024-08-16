#include "pch.hpp"
#include "OpenGLVertexArray.hpp"
#include <glad/glad.h>

namespace ProjectA {
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

		CORE_ASSERT(false, "Unknown ShaderType");

		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &id);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &id);
	}

	void OpenGLVertexArray::bind() const {
		glBindVertexArray(id);
	}

	void OpenGLVertexArray::unbind() const {
		glBindVertexArray(0);
	}

	const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::getVertexBuffers() const {
		return vertexBuffers;
	}

	const Ref<IndexBuffer>& OpenGLVertexArray::getIndexBuffer() const {
		return indexBuffer;
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has no layout");

		bind();
		vertexBuffer->bind();

		uint32_t attributeIndex = 0;

		const auto& layout = vertexBuffer->getLayout();

		for (auto& element : layout) {
			glEnableVertexAttribArray(attributeIndex);
			glVertexAttribPointer(
				attributeIndex,
				getShaderTypeComponentCount(element.type),
				getShaderType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)element.offset
			);

			attributeIndex++;
		}

		vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		bind();
		indexBuffer->bind();

		this->indexBuffer = indexBuffer;
	}
}