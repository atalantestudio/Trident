#include "pch.hpp"
#include <fstream>
#include <glad/glad.h>
#include "OpenGLShader.hpp"

namespace ProjectA {
	static GLenum shaderTypeFromString(const std::string& shaderType) {
		if (shaderType == "vertex") {
			return GL_VERTEX_SHADER;
		}

		if (shaderType == "fragment") {
			return GL_FRAGMENT_SHADER;
		}

		CORE_ASSERT(false, "Unhandled shader type \"{0}\".", shaderType);

		return 0;
	}

	std::string OpenGLShader::readFile(const std::string& path) const {
		std::ifstream input(path, std::ios::in | std::ios::binary);

		if (!input) {
			LOG_CORE_FATAL("Could not open file \"{0}\".", path);

			return 0;
		}

		std::string result;

		input.seekg(0, std::ios::end);

		result.resize(input.tellg());

		input.seekg(0, std::ios::beg);
		input.read(&result[0], result.size());
		input.close();

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preprocess(const std::string& source) const {
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* shaderTypeToken = "#type";
		size_t shaderTypeTokenLength = strlen(shaderTypeToken);
		size_t position = source.find(shaderTypeToken, 0);

		while (position != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", position);

			CORE_ASSERT(eol == std::string::npos, "Shader syntax error detected.");

			size_t begin = position + shaderTypeTokenLength + 1;

			std::string shaderTypeIdentifier = source.substr(begin, eol - begin);
			GLenum shaderType = shaderTypeFromString(shaderTypeIdentifier);

			size_t nextLinePosition = source.find_first_not_of("\r\n", eol);

			position = source.find(shaderTypeToken, nextLinePosition);
			std::string shaderSource = source.substr(nextLinePosition, position - (nextLinePosition == std::string::npos ? source.size() - 1 : nextLinePosition));

			shaderSources[shaderType] = shaderSource;
		}

		return shaderSources;
	}

	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
		GLuint program = glCreateProgram();

		CORE_ASSERT(shaderSources.size() > 2, "Only 2-shader files are supported.");

		std::array<GLuint, 2> shaders;
		uint32_t index = 0;

		for (auto&& [shaderType, shaderSource] : shaderSources) {
			const GLchar* source = shaderSource.c_str();
			GLint compileStatus = 0;

			GLuint shader = glCreateShader(shaderType);

			glShaderSource(shader, 1, &source, 0);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

			if (compileStatus == GL_FALSE) {
				GLint maxLength = 0;

				glGetShaderiv(shader, GL_COMPILE_STATUS, &maxLength);

				std::vector<GLchar> infoLog(maxLength);

				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
				glDeleteShader(shader);

				LOG_CORE_FATAL("Shader compilation failed: {0}", infoLog.data());

				break;
			}

			glAttachShader(program, shader);

			shaders[index] = shader;

			index++;
		}

		glLinkProgram(program);

		GLint linkStatus = 0;

		glGetProgramiv(program, GL_LINK_STATUS, (int*) &linkStatus);

		if (linkStatus == GL_FALSE) {
			GLint maxLength = 0;

			glGetProgramiv(program, GL_LINK_STATUS, &maxLength);

			std::vector<GLchar> infoLog(maxLength);

			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			for (auto shader : shaders) {
				glDeleteShader(shader);
			}

			glDeleteProgram(program);

			LOG_CORE_FATAL("Program compilation failed: {0}", infoLog.data());

			return;
		}

		for (auto shader : shaders) {
			glDetachShader(program, shader);
		}

		this->program = program;
	}

	OpenGLShader::OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name) : program(0), name(name) {
		std::unordered_map<GLenum, std::string> shaderSources;

		shaderSources[GL_VERTEX_SHADER] = vertexShaderSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentShaderSource;

		compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& path) : program(0) {
		compile(preprocess(readFile(path)));

		size_t lastSlash = path.find_last_of("/\\");

		if (lastSlash == std::string::npos) {
			lastSlash = 0;
		} else {
			lastSlash += 1;
		}

		size_t lastDot = path.rfind('.');
		size_t count = 0;

		if (lastDot == std::string::npos) {
			count = path.size() - lastSlash;
		} else {
			count = lastDot - lastSlash;
		}

		std::string name = path.substr(lastSlash, count);

		this->name = name;
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

	const std::string& OpenGLShader::getName() const {
		return name;
	}

	void OpenGLShader::uploadUniform(const std::string& name, int value) const {
		GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			return;
		}

		glUniform1i(location, value);
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