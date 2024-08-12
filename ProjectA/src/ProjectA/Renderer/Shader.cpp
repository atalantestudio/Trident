#include "pch.hpp"
#include "Shader.hpp"
#include <glad/glad.h>

namespace ProjectA {
	Shader::Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
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

	Shader::~Shader() {
		glDeleteProgram(program);
	}

	void Shader::bind() const {
		glUseProgram(program);
	}

	void Shader::unbind() const {
		glUseProgram(0);
	}

	void Shader::uploadUniform(const std::string& name, const vec4<float>& vector) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		float entries[4] = {vector.x, vector.y, vector.z, vector.w};

		glUniform4fv(location, 1, entries);
	}

	void Shader::uploadUniform(const std::string& name, const mat4x4<float>& matrix) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		glUniformMatrix4fv(location, 1, GL_FALSE, matrix.entries);
	}
}