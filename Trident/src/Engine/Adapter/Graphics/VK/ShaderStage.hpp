// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ShaderStage.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::VK {
	inline VkShaderStageFlagBits getShaderStageFlagBits(ShaderStage shaderStage) {
		VkShaderStageFlags shaderStageFlags = 0;

		if (shaderStage & ShaderStage::VERTEX) {
			shaderStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if (shaderStage & ShaderStage::FRAGMENT) {
			shaderStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if (shaderStage & ShaderStage::COMPUTE) {
			shaderStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		if (shaderStage & ShaderStage::RAY_GENERATION) {
			shaderStageFlags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		}

		if (shaderStage & ShaderStage::RAY_CLOSEST_HIT) {
			shaderStageFlags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		}

		if (shaderStage & ShaderStage::RAY_MISS) {
			shaderStageFlags |= VK_SHADER_STAGE_MISS_BIT_KHR;
		}

		if (shaderStage & ShaderStage::RAY_ANY_HIT) {
			shaderStageFlags |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		}

		if (shaderStage & ShaderStage::RAY_INTERSECTION) {
			shaderStageFlags |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		}

		ADAPTER_ASSERT(shaderStageFlags != 0, "Undefined shader stage flag.");

		return static_cast<VkShaderStageFlagBits>(shaderStageFlags);
	}
}