#pragma once

#include <vulkan/vulkan.h>

namespace ProjectA {
	static VkShaderStageFlagBits getVkShaderStageFlagBits(const uint32 shaderStage) {
		static constexpr VkShaderStageFlagBits vkShaderStageFlagBits[] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		return vkShaderStageFlagBits[shaderStage];
	}
}