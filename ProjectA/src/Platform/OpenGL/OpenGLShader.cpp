#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLShader.hpp"
#include "Platform/OpenGL/OpenGLShaderModule.hpp"

namespace ProjectA {
	// TODO: Change for enum
	static ShaderStage shaderStageFromString(const std::string& shaderStage) {
		PROFILE_SCOPE(__FUNCSIG__);

		if (shaderStage == "vertex") {
			return ShaderStage::VERTEX;
		}

		if (shaderStage == "fragment") {
			return ShaderStage::FRAGMENT;
		}

		ENGINE_ASSERT(false, "Unhandled shader stage \"[]\".", shaderStage);

		return ShaderStage::VERTEX;
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

	std::unordered_map<ShaderStage, std::string> OpenGLShader::preprocess(const std::string& code) const {
		PROFILE_SCOPE(__FUNCSIG__);

		std::unordered_map<ShaderStage, std::string> shaderSources;

		constexpr char* shaderStageToken = "#type";

		std::size_t shaderStageTokenLength = strlen(shaderStageToken);
		std::size_t position = code.find(shaderStageToken, 0);

		while (position != std::string::npos) {
			std::size_t eol = code.find_first_of("\r\n", position);

			const bool found = eol == std::string::npos;

			ENGINE_ASSERT(!found, "Shader syntax error detected.");

			std::size_t begin = position + shaderStageTokenLength + 1;

			// TODO: Don't automatically determine shader type
			const std::string shaderStageIdentifier = code.substr(begin, eol - begin);
			const ShaderStage shaderStage = shaderStageFromString(shaderStageIdentifier);

			std::size_t nextLinePosition = code.find_first_not_of("\r\n", eol);

			position = code.find(shaderStageToken, nextLinePosition);
			std::string shaderCode = code.substr(nextLinePosition, position - (nextLinePosition == std::string::npos ? code.size() - 1 : nextLinePosition));

			shaderSources[shaderStage] = shaderCode;
		}

		return shaderSources;
	}

	void OpenGLShader::compile(const std::unordered_map<ShaderStage, std::string>& shaderCodes) {
		PROFILE_SCOPE(__FUNCSIG__);

		const bool sourceCountCheck = shaderCodes.size() > 2;

		ENGINE_ASSERT(!sourceCountCheck, "Only 2-shader files are supported.");

		const GLuint program = glCreateProgram();
		const OpenGLShaderModule vertexShaderModule(ShaderStage::VERTEX, shaderCodes.at(ShaderStage::VERTEX));
		const OpenGLShaderModule fragmentShaderModule(ShaderStage::FRAGMENT, shaderCodes.at(ShaderStage::FRAGMENT));

		glAttachShader(program, vertexShaderModule.object);
		glAttachShader(program, fragmentShaderModule.object);

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

			vertexShaderModule.~OpenGLShaderModule();
			fragmentShaderModule.~OpenGLShaderModule();

			glDeleteProgram(program);

			ENGINE_FATAL("Program linking failed: []", infoLog.data());

			return;
		}

		ENGINE_TRACE("Linked shader program successfully.");

		glDetachShader(program, vertexShaderModule.object);
		glDetachShader(program, fragmentShaderModule.object);

		this->program = program;
	}

	OpenGLShader::OpenGLShader(const std::string& vertexShaderCode, const std::string& fragmentShaderCode, const std::string& name) : program(0), name(name) {
		PROFILE_SCOPE(__FUNCSIG__);

		std::unordered_map<ShaderStage, std::string> shaderCodes;

		shaderCodes[ShaderStage::VERTEX] = vertexShaderCode;
		shaderCodes[ShaderStage::FRAGMENT] = fragmentShaderCode;

		compile(shaderCodes);
	}

	OpenGLShader::OpenGLShader(const std::string& path) : program(0) {
		PROFILE_SCOPE(__FUNCSIG__);

		compile(preprocess(readFile(path)));

		std::size_t lastSlash = path.find_last_of("/\\");

		if (lastSlash == std::string::npos) {
			lastSlash = 0;
		} else {
			lastSlash += 1;
		}

		std::size_t lastDot = path.rfind('.');
		std::size_t count = 0;

		if (lastDot == std::string::npos) {
			count = path.size() - lastSlash;
		} else {
			count = lastDot - lastSlash;
		}

		name = path.substr(lastSlash, count);
	}

	OpenGLShader::~OpenGLShader() {
		PROFILE_SCOPE(__FUNCSIG__);

		glDeleteProgram(program);
	}

	const std::string& OpenGLShader::getName() const {
		return name;
	}

	void OpenGLShader::bind() const {
		PROFILE_SCOPE(__FUNCSIG__);

		glUseProgram(program);
	}

	void OpenGLShader::unbind() const {		
		PROFILE_SCOPE(__FUNCSIG__);

		glUseProgram(0);
	}

	void OpenGLShader::setUniform(const std::string& name, int32 value) const {
		PROFILE_SCOPE(__FUNCSIG__);

		const GLint location = glGetUniformLocation(program, name.c_str());

		// TODO: DRY
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
			ENGINE_WARN("Location not found for uniform \"[]\". Uniform is either unused or non-existing.", name);
		}

		GLfloat entries[16]{};

		_mm_store_ps(&entries[0], value[0].m);
		_mm_store_ps(&entries[4], value[1].m);
		_mm_store_ps(&entries[8], value[2].m);
		_mm_store_ps(&entries[12], value[3].m);

		glUniformMatrix4fv(location, 1, GL_FALSE, entries);
	}
}