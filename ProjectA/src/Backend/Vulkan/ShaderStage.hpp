#pragma once

namespace ProjectA {
	static VkShaderStageFlagBits getVkShaderStageFlagBits(ShaderStage shaderStage) {
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

		DEBUG_ASSERT(vkShaderStageFlags != 0);

		return static_cast<VkShaderStageFlagBits>(vkShaderStageFlags);
	}
}