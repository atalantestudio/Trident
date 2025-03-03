#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"
#include "Platform/OpenGL/OpenGLShaderModule.hpp"
#include "Platform/OpenGL/VertexInputAttributeType.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		program(glCreateProgram()),
		primitiveTopology(primitiveTopologies[static_cast<uint32>(descriptor.primitiveTopology)])
	{
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLRenderPipeline]");

		glCreateVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		const GLuint vertexShaderModule = reinterpret_cast<const OpenGLShaderModule*>(descriptor.vertexShaderModule.get())->object;
		const GLuint fragmentShaderModule = reinterpret_cast<const OpenGLShaderModule*>(descriptor.fragmentShaderModule.get())->object;

		glAttachShader(program, vertexShaderModule);
		glAttachShader(program, fragmentShaderModule);
		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == 0) {
			GLint infoLogLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::vector<GLchar> infoLog(infoLogLength);

			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data());

			ENGINE_FATAL("Program linking failed.\n[]", infoLog.data());

			throw std::runtime_error("Failed to link OpenGL program.");
		}

		glDetachShader(program, vertexShaderModule);
		glDetachShader(program, fragmentShaderModule);

		for (const VertexAttribute& vertexInputAttribute : descriptor.vertexBufferLayout) {
			const GLenum type = getGlType(vertexInputAttribute.type);
			const GLint size = getGlSize(vertexInputAttribute.type);
			const void* offset = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(vertexInputAttribute.offset));

			glEnableVertexAttribArray(vertexInputAttribute.location);

			switch (type) {
				case GL_FLOAT:
					glVertexAttribFormat(vertexInputAttribute.location, size, type, vertexInputAttribute.normalized, vertexInputAttribute.offset);
					//glVertexAttribPointer(vertexInputAttribute.location, size, type, vertexInputAttribute.normalized, descriptor.vertexBufferLayout.stride, offset);

					break;
				case GL_BYTE:
				case GL_UNSIGNED_BYTE:
				case GL_SHORT:
				case GL_UNSIGNED_SHORT:
				case GL_INT:
				case GL_UNSIGNED_INT:
					glVertexAttribIFormat(vertexInputAttribute.location, size, type, vertexInputAttribute.offset);
					//glVertexAttribIPointer(vertexInputAttribute.location, size, type, descriptor.vertexBufferLayout.stride, offset);

					break;
				case GL_DOUBLE:
					glVertexAttribLFormat(vertexInputAttribute.location, size, type, vertexInputAttribute.offset);
					//glVertexAttribLPointer(vertexInputAttribute.location, size, type, descriptor.vertexBufferLayout.stride, offset);

					break;
			}

			glVertexAttribBinding(vertexInputAttribute.location, vertexInputAttribute.binding);
		}
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[~OpenGLRenderPipeline]");

		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vertexArray);
	}

	/*void OpenGLRenderPipeline::setUniform(const std::string& name, int32 value) const {
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
		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_ERROR("Location not found for uniform \"[]\". Uniform is either unused or non-existing.", name);

			// TODO: Throw instead of returning
			return -1;
		}

		return location;
	}*/
}