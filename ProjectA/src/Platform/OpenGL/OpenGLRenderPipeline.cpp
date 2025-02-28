#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"
#include "Platform/OpenGL/OpenGLShaderModule.hpp"
#include "Platform/OpenGL/VertexInputAttributeType.hpp"

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
		PROFILE_SCOPE(FUNC_SIG);

		glBindVertexArray(vertexArrayObject);
		glUseProgram(programObject);
	}

	void OpenGLRenderPipeline::unbind() const {		
		PROFILE_SCOPE(FUNC_SIG);

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
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(vertexBuffer->getLayout().vertexAttributes.size(), "Vertex buffer has no layout.");

		bind();

		vertexBuffer->bind();

		const BufferLayout& layout = vertexBuffer->getLayout();

		for (const VertexAttribute& vertexInputAttribute : layout) {
			const GLenum type = getGlType(vertexInputAttribute.type);
			const GLint size = getGlSize(vertexInputAttribute.type);
			const void* offset = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(vertexInputAttribute.offset));

			glEnableVertexAttribArray(vertexInputAttribute.location);

			switch (type) {
				case GL_FLOAT:
					glVertexAttribPointer(vertexInputAttribute.location, size, type, vertexInputAttribute.normalized, layout.stride, offset);

					break;
				case GL_BYTE:
				case GL_UNSIGNED_BYTE:
				case GL_SHORT:
				case GL_UNSIGNED_SHORT:
				case GL_INT:
				case GL_UNSIGNED_INT:
					glVertexAttribIPointer(vertexInputAttribute.location, size, type, layout.stride, offset);

					break;
				case GL_DOUBLE:
					glVertexAttribLPointer(vertexInputAttribute.location, size, type, layout.stride, offset);

					break;
			}
		}

		vertexBuffers.emplace_back(vertexBuffer);
	}

	void OpenGLRenderPipeline::setIndexBuffer(const ref<IndexBuffer>& indexBuffer) {
		PROFILE_SCOPE(FUNC_SIG);

		bind();

		indexBuffer->bind();

		this->indexBuffer = indexBuffer;
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, int32 value) const {
		PROFILE_SCOPE(FUNC_SIG);

		const GLint location = getUniformLocation(name);

		glUniform1i(location, value);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const int32* value, uint32 count) const {
		PROFILE_SCOPE(FUNC_SIG);

		const GLint location = getUniformLocation(name);

		glUniform1iv(location, count, value);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, float32 value) const {
		PROFILE_SCOPE(FUNC_SIG);

		const GLint location = getUniformLocation(name);

		glUniform1f(location, value);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const vec2<float32>& value) const {
		PROFILE_SCOPE(FUNC_SIG);

		const GLint location = getUniformLocation(name);

		glUniform2f(location, value[0], value[1]);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const vec3<float32>& value) const {
		PROFILE_SCOPE(FUNC_SIG);

		const GLint location = getUniformLocation(name);

		glUniform3f(location, value[0], value[1], value[2]);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const vec4<float32>& value) const {
		PROFILE_SCOPE(FUNC_SIG);

		const GLint location = getUniformLocation(name);

		glUniform4f(location, value[0], value[1], value[2], value[3]);
	}

	void OpenGLRenderPipeline::setUniform(const std::string& name, const mat4x4<float32>& value) const {
		PROFILE_SCOPE(FUNC_SIG);

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