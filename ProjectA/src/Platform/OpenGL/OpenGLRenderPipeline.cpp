#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"
#include "Platform/OpenGL/OpenGLShaderModule.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		programObject(glCreateProgram()),
		primitiveTopology(primitiveTopologies[static_cast<uint32>(descriptor.primitiveTopology)])
	{
		glCreateVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glAttachShader(programObject, ((OpenGLShaderModule*) descriptor.vertexShaderModule.get())->object);
		glAttachShader(programObject, ((OpenGLShaderModule*) descriptor.fragmentShaderModule.get())->object);
		glLinkProgram(programObject);

		GLint linkStatus;

		glGetProgramiv(programObject, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == 0) {
			GLint infoLogLength;

			glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::vector<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(programObject, infoLogLength, nullptr, infoLog.data());

			//descriptor.vertexShaderModule.~ShaderModule();
			//descriptor.fragmentShaderModule.~ShaderModule();

			this->~OpenGLRenderPipeline();

			ENGINE_FATAL("Program linking failed.\n[]", infoLog.data());

			// TODO: Throw instead of returning
			return;
		}

		glDetachShader(programObject, ((OpenGLShaderModule*) descriptor.vertexShaderModule.get())->object);
		glDetachShader(programObject, ((OpenGLShaderModule*) descriptor.fragmentShaderModule.get())->object);

		ENGINE_TRACE("Linked shader program successfully.");
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		glDeleteProgram(programObject);
		glDeleteVertexArrays(1, &vertexArrayObject);
	}

	void OpenGLRenderPipeline::bind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glBindVertexArray(vertexArrayObject);
		glUseProgram(programObject);
	}

	void OpenGLRenderPipeline::unbind() const {		
		PROFILE_SCOPE(__FUNCSIG__);

		glUseProgram(0);
		glBindVertexArray(0);
	}

	const std::vector<ref<VertexBuffer>>& OpenGLRenderPipeline::getVertexBuffers() const {
		return vertexBuffers;
	}

	const ref<IndexBuffer>& OpenGLRenderPipeline::getIndexBuffer() const {
		return indexBuffer;
	}

	void OpenGLRenderPipeline::addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) {
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

	void OpenGLRenderPipeline::setIndexBuffer(const ref<IndexBuffer>& indexBuffer) {
		PROFILE_SCOPE(__FUNCSIG__);

		bind();

		indexBuffer->bind();

		this->indexBuffer = indexBuffer;
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, int32 value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = getUniformLocation(name);

		glUniform1i(location, value);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const int32* value, uint32 count) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = getUniformLocation(name);

		glUniform1iv(location, count, value);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, float32 value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = getUniformLocation(name);

		glUniform1f(location, value);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const lyah::vec2<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = getUniformLocation(name);

		glUniform2f(location, value[0], value[1]);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const lyah::vec3<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = getUniformLocation(name);

		glUniform3f(location, value[0], value[1], value[2]);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const lyah::vec4<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = getUniformLocation(name);

		glUniform4f(location, value[0], value[1], value[2], value[3]);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const lyah::mat4x4<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = getUniformLocation(name);

		GLfloat entries[16]{};

		_mm_store_ps(&entries[0], value[0].m);
		_mm_store_ps(&entries[4], value[1].m);
		_mm_store_ps(&entries[8], value[2].m);
		_mm_store_ps(&entries[12], value[3].m);

		glUniformMatrix4fv(location, 1, GL_FALSE, entries);
	}

	GLint OpenGLRenderPipeline::getUniformLocation(const std::string& name) const {
		const GLint location = glGetUniformLocation(programObject, name.c_str());

		if (location == -1) {
			ENGINE_ERROR("Location not found for uniform \"[]\". Uniform is either unused or non-existing.", name);

			// TODO: Throw instead of returning
			return -1;
		}

		return location;
	}
}