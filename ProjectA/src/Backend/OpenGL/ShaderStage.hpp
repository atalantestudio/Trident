#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	static GLenum getGlShaderStage(ShaderStage shaderStage) {
		static constexpr GLenum glShaderStages[] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
		};

		return glShaderStages[static_cast<uint8>(shaderStage)];
	}
}