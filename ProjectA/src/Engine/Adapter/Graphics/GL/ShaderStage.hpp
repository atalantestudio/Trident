// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/ShaderStage.hpp"

namespace ProjectA::GL {
	// TODO: Treat as flag.
	inline GLenum getShaderStage(ShaderStage shaderStage) {
		if (shaderStage == ShaderStage::VERTEX) {
			return GL_VERTEX_SHADER;
		}

		if (shaderStage == ShaderStage::FRAGMENT) {
			return GL_FRAGMENT_SHADER;
		}

		if (shaderStage == ShaderStage::COMPUTE) {
			return GL_COMPUTE_SHADER;
		}

		ADAPTER_ASSERT(false, "Unsupported shader stage.");

		return 0;
	}
}