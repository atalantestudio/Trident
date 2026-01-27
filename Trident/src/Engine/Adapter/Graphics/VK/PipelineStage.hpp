// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PipelineStage.hpp"

namespace Trident::VK {
	inline VkPipelineStageFlagBits getPipelineStageFlagBits(PipelineStage pipelineStage) {
		static constexpr VkPipelineStageFlagBits vkPipelineStages[] = {
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		};

		return vkPipelineStages[static_cast<uint8>(pipelineStage)];
	}
}