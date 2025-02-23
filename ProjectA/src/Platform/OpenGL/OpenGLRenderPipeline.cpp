#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRenderPipeline.hpp"
#include "Platform/OpenGL/OpenGLShaderModule.hpp"

namespace ProjectA {
	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor) :
		programObject(glCreateProgram()),
		primitiveTopology(primitiveTopologies[static_cast<uint32>(descriptor.primitiveTopology)])
	{
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
	}

	void OpenGLRenderPipeline::bind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glUseProgram(programObject);
	}

	void OpenGLRenderPipeline::unbind() const {		
		PROFILE_SCOPE(__FUNCSIG__);

		glUseProgram(0);
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