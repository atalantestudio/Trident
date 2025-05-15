#pragma once

namespace ProjectA {
	static VkShaderStageFlagBits getVkShaderStageFlagBits(ShaderStage shaderStage) {
		VkShaderStageFlags vkShaderStage = 0;

		if (shaderStage & ShaderStage::VERTEX) {
			vkShaderStage |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (shaderStage & ShaderStage::FRAGMENT) {
			vkShaderStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (shaderStage & ShaderStage::COMPUTE) {
			vkShaderStage |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		return static_cast<VkShaderStageFlagBits>(vkShaderStage);
	}
}