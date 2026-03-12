// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PipelineStage.hpp"

namespace Trident::VK {
	inline VkPipelineStageFlagBits getPipelineStageFlagBits(PipelineStage pipelineStage) {
		static constexpr VkPipelineStageFlagBits PIPELINE_STAGES[] = {
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		};

		ADAPTER_ASSERT(pipelineStage < PipelineStage::count, "Invalid pipeline stage.");

		return PIPELINE_STAGES[static_cast<uint32>(pipelineStage)];
	}
}