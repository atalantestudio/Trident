#pragma once

namespace ProjectA {
	// TODO: Treat as flag.
	static GLenum getGlShaderStage(ShaderStage shaderStage) {
		if (shaderStage == ShaderStage::VERTEX) {
			return GL_VERTEX_SHADER;
		}

		if (shaderStage == ShaderStage::FRAGMENT) {
			return GL_FRAGMENT_SHADER;
		}

		if (shaderStage == ShaderStage::COMPUTE) {
			return GL_COMPUTE_SHADER;
		}

		DEBUG_ASSERT(false);

		return 0;
	}
}