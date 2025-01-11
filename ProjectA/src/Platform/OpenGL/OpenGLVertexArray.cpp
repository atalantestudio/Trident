#include "pch.hpp"

#include <glad/glad.h>

#include "OpenGLVertexArray.hpp"

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

	OpenGLVertexArray::OpenGLVertexArray() : vertexBufferIndex(0) {
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

	const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::getVertexBuffers() const {
		return vertexBuffers;
	}

	const Ref<IndexBuffer>& OpenGLVertexArray::getIndexBuffer() const {
		return indexBuffer;
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		PROFILE_SCOPE(__FUNCSIG__);

		CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has no layout");

		bind();
		vertexBuffer->bind();

		const auto& layout = vertexBuffer->getLayout();

		for (auto& element : layout) {
			glEnableVertexAttribArray(vertexBufferIndex);
			glVertexAttribPointer(
				vertexBufferIndex,
				getShaderTypeComponentCount(element.type),
				getShaderType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*) (std::intptr_t) element.offset
			);

			vertexBufferIndex++;
		}

		vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		PROFILE_SCOPE(__FUNCSIG__);

		bind();
		indexBuffer->bind();

		this->indexBuffer = indexBuffer;
	}
}