#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	static VkShaderStageFlagBits getVkShaderStageFlagBits(ShaderStage shaderStage) {
		static constexpr VkShaderStageFlagBits vkShaderStageFlagBits[] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		return vkShaderStageFlagBits[static_cast<uint8>(shaderStage)];
	}
}