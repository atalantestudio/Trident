#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLShader.hpp"

namespace ProjectA {
	// TODO: Refactor
	static GLenum shaderTypeFromString(const std::string& shaderType) {
		PROFILE_SCOPE(__FUNCSIG__);

		if (shaderType == "vertex") {
			return GL_VERTEX_SHADER;
		}

		if (shaderType == "fragment") {
			return GL_FRAGMENT_SHADER;
		}

		ENGINE_ASSERT(false, "Unhandled shader type \"{0}\".", shaderType);

		return 0;
	}

	std::string OpenGLShader::readFile(const std::string& path) const {
		PROFILE_SCOPE(__FUNCSIG__);

		std::ifstream input(path, std::ios::in | std::ios::binary);

		if (!input) {
			ENGINE_FATAL("Could not open file \"[]\".", path);

			ENGINE_ASSERT(false, "Could not open file");
		}

		std::string result;

		input.seekg(0, std::ios::end);

		const std::size_t size = input.tellg();

		if (size == -1) {
			ENGINE_FATAL("Could not read file \"[]\".", path);

			ENGINE_ASSERT(false, "Could not read file");
		}

		result.resize(size);

		input.seekg(0, std::ios::beg);
		input.read(&result[0], size);
		input.close();

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preprocess(const std::string& source) const {
		PROFILE_SCOPE(__FUNCSIG__);

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* shaderTypeToken = "#type";
		size_t shaderTypeTokenLength = strlen(shaderTypeToken);
		size_t position = source.find(shaderTypeToken, 0);

		while (position != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", position);

			const bool found = eol == std::string::npos;

			ENGINE_ASSERT(!found, "Shader syntax error detected.");

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
		PROFILE_SCOPE(__FUNCSIG__);

		GLuint program = glCreateProgram();

		const bool sourceCountCheck = shaderSources.size() > 2;

		ENGINE_ASSERT(!sourceCountCheck, "Only 2-shader files are supported.");

		std::array<GLuint, 2> shaders{};
		uint32_t index = 0;

		for (auto&& [shaderType, shaderSource] : shaderSources) {
			const GLchar* source = shaderSource.c_str();

			GLuint shader = glCreateShader(shaderType);

			glShaderSource(shader, 1, &source, 0);
			glCompileShader(shader);

			GLint compileStatus;

			glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

			if (compileStatus == GL_FALSE) {
				GLint maxLength;

				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength == 0) {
					ENGINE_FATAL("Shader compilation failed (no info log provided)");

					break;
				}

				std::vector<GLchar> infoLog(maxLength);

				glGetShaderInfoLog(shader, maxLength, nullptr, infoLog.data());
				glDeleteShader(shader);

				ENGINE_FATAL("Shader compilation failed: []", infoLog.data());

				break;
			}

			glAttachShader(program, shader);

			shaders[index] = shader;

			index++;
		}

		glLinkProgram(program);

		GLint linkStatus;

		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == GL_FALSE) {
			GLint maxLength;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			if (maxLength == 0) {
				ENGINE_FATAL("Program linking failed (no info log provided)");

				return;
			}

			std::vector<GLchar> infoLog(maxLength);

			glGetProgramInfoLog(program, maxLength, nullptr, infoLog.data());

			for (GLuint shader : shaders) {
				glDeleteShader(shader);
			}

			glDeleteProgram(program);

			ENGINE_FATAL("Program linking failed: []", infoLog.data());

			return;
		}

		ENGINE_INFO("Shader program compiled and linked successfully.");

		for (auto shader : shaders) {
			glDetachShader(program, shader);
			glDeleteShader(shader);
		}

		this->program = program;
	}

	OpenGLShader::OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name) : program(0), name(name) {
		PROFILE_SCOPE(__FUNCSIG__);

		std::unordered_map<GLenum, std::string> shaderSources;

		shaderSources[GL_VERTEX_SHADER] = vertexShaderSource;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentShaderSource;

		compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& path) : program(0) {
		PROFILE_SCOPE(__FUNCSIG__);

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
		PROFILE_SCOPE(__FUNCSIG__);

		glDeleteProgram(program);
	}

	void OpenGLShader::bind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glUseProgram(program);
	}

	void OpenGLShader::unbind() const {		
		PROFILE_SCOPE(__FUNCSIG__);

		glUseProgram(0);
	}

	const std::string& OpenGLShader::getName() const {
		return name;
	}

	void OpenGLShader::setUniform(const std::string& name, int32 value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing.", name);
		}

		glUniform1i(location, value);
	}

	void OpenGLShader::setUniform(const std::string& name, const int32* value, uint32 count) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing.", name);
		}

		glUniform1iv(location, count, value);
	}

	void OpenGLShader::setUniform(const std::string& name, float32 value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing", name);
		}

		glUniform1f(location, value);
	}

	void OpenGLShader::setUniform(const std::string& name, const lyah::vec2<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing", name);
		}

		glUniform2f(location, value[0], value[1]);
	}

	void OpenGLShader::setUniform(const std::string& name, const lyah::vec3<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing", name);
		}

		glUniform3f(location, value[0], value[1], value[2]);
	}

	void OpenGLShader::setUniform(const std::string& name, const lyah::vec4<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing", name);
		}

		glUniform4f(location, value[0], value[1], value[2], value[3]);
	}

	void OpenGLShader::setUniform(const std::string& name, const lyah::mat4x4<float32>& value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		if (location == -1) {
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing", name);
		}

		GLfloat entries[16]{};

		_mm_store_ps(&entries[0], value[0].m);
		_mm_store_ps(&entries[4], value[1].m);
		_mm_store_ps(&entries[8], value[2].m);
		_mm_store_ps(&entries[12], value[3].m);

		glUniformMatrix4fv(location, 1, GL_FALSE, entries);
	}
}