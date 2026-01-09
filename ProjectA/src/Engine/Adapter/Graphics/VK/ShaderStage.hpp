// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderStage.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA::VK {
	inline VkShaderStageFlagBits getShaderStageFlagBits(ShaderStage shaderStage) {
		VkShaderStageFlags vkShaderStageFlags = 0;

		if (shaderStage & ShaderStage::VERTEX) {
			vkShaderStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (shaderStage & ShaderStage::FRAGMENT) {
			vkShaderStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (shaderStage & ShaderStage::COMPUTE) {
			vkShaderStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		ADAPTER_ASSERT(vkShaderStageFlags != 0, "Undefined shader stage flag.");

		return static_cast<VkShaderStageFlagBits>(vkShaderStageFlags);
	}
}