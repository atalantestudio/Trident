#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	static GLenum getGlShaderStage(ShaderStage shaderStage) {
		// TODO: Combine multiple shader stages.
		static constexpr GLenum glShaderStages[] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
			GL_COMPUTE_SHADER,
		};

		return glShaderStages[static_cast<uint8>(shaderStage)];
	}
}