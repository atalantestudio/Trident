#pragma once

namespace ProjectA {
	static VkShaderStageFlagBits getVkShaderStageFlagBits(ShaderStage shaderStage) {
		if (shaderStage == ShaderStage::VERTEX) {
			return VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (shaderStage == ShaderStage::FRAGMENT) {
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (shaderStage == ShaderStage::COMPUTE) {
			return VK_SHADER_STAGE_COMPUTE_BIT;
		}

		DEBUG_ASSERT(false);

		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}
}