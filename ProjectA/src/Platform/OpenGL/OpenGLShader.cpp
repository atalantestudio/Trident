#include "pch.hpp"
#include "OpenGLShader.hpp"
#include <glad/glad.h>

namespace ProjectA {
	OpenGLShader::OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* source = vertexShaderSource.c_str();
		GLint compileStatus = 0;

		glShaderSource(vertexShader, 1, &source, 0);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_FALSE) {
			GLint maxLength = 0;

			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &maxLength);

			std::vector<GLchar> infoLog(maxLength);

			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(vertexShader);

			LOG_CORE_ERROR("Vertex shader compilation failed: {0}", infoLog.data());

			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = fragmentShaderSource.c_str();

		glShaderSource(fragmentShader, 1, &source, 0);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_FALSE) {
			GLint maxLength = 0;

			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &maxLength);

			std::vector<GLchar> infoLog(maxLength);

			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			LOG_CORE_ERROR("Fragment shader compilation failed: {0}", infoLog.data());

			return;
		}

		program = glCreateProgram();

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint linkStatus = 0;

		glGetProgramiv(program, GL_LINK_STATUS, (int*) &linkStatus);

		if (linkStatus == GL_FALSE) {
			GLint maxLength = 0;

			glGetProgramiv(program, GL_LINK_STATUS, &maxLength);

			std::vector<GLchar> infoLog(maxLength);

			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteProgram(program);

			LOG_CORE_ERROR("Program compilation failed: {0}", infoLog.data());

			return;
		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(program);
	}

	void OpenGLShader::bind() const {
		glUseProgram(program);
	}

	void OpenGLShader::unbind() const {
		glUseProgram(0);
	}

	void OpenGLShader::uploadUniform(const std::string& name, int value) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniform(const std::string& name, float value) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniform(const std::string& name, const vec3<float>& value) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		glUniform3f(location, value[0], value[1], value[2]);
	}

	void OpenGLShader::uploadUniform(const std::string& name, const vec4<float>& value) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::uploadUniform(const std::string& name, const mat4x4<float>& value) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		glUniformMatrix4fv(location, 1, GL_FALSE, value.entries);
	}
}