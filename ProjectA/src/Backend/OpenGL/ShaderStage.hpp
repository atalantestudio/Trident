#pragma once

namespace ProjectA {
	static GLenum getGlShaderStage(ShaderStage shaderStage) {
		GLenum glShaderStage = 0;

		if (shaderStage & ShaderStage::VERTEX) {
			glShaderStage = GL_VERTEX_SHADER;
		}

		if (shaderStage & ShaderStage::FRAGMENT) {
			glShaderStage = GL_FRAGMENT_SHADER;
		}

		if (shaderStage & ShaderStage::COMPUTE) {
			glShaderStage = GL_COMPUTE_SHADER;
		}

		ENGINE_ASSERT(glShaderStage != 0, "Invalid OpenGL shader stage.");

		return glShaderStage;
	}
}