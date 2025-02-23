#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLVertexArray.hpp"

namespace ProjectA {
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

		ENGINE_ASSERT(vertexBuffer->getLayout().attributes.size(), "Vertex buffer has no layout.");

		bind();

		vertexBuffer->bind();

		const BufferLayout& layout = vertexBuffer->getLayout();

		GLuint attributeIndex = 0;

		for (const BufferAttribute& attribute : layout) {
			switch (attribute.glType) {
				case GL_FLOAT:
					if (attribute.componentCount <= 4) {
						// Scalar/vector
						glEnableVertexAttribArray(attributeIndex);
						glVertexAttribPointer(
							attributeIndex,
							static_cast<GLint>(attribute.componentCount),
							attribute.glType,
							attribute.normalized ? GL_TRUE : GL_FALSE,
							layout.stride,
							(const void*) attribute.offset
						);

						attributeIndex++;

						break;
					}
					else {
						// Matrix
						const GLboolean normalized = attribute.normalized ? GL_TRUE : GL_FALSE;

						for (uint32 i = 0; i < attribute.componentCount; i++) {
							glEnableVertexAttribArray(attributeIndex);
							glVertexAttribPointer(
								attributeIndex,
								static_cast<GLint>(attribute.componentCount),
								attribute.glType,
								normalized,
								layout.stride,
								(const void*) (sizeof(float32) * attribute.componentCount * i)
							);
							glVertexAttribDivisor(attributeIndex, 1);

							attributeIndex++;
						}

						break;
					}
				case GL_BOOL:
				case GL_INT:
					glEnableVertexAttribArray(attributeIndex);
					glVertexAttribIPointer(
						attributeIndex,
						static_cast<GLint>(attribute.componentCount),
						attribute.glType,
						layout.stride,
						(const void*) attribute.offset
					);

					attributeIndex++;

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