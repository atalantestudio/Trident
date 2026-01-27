// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class PipelineStage : uint8 {
		ALL_COMMANDS = 0,

		// From the Vulkan specification:
		// VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT is equivalent to VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
		// with VkAccessFlags set to 0 when specified in the second synchronization scope,
		// but specifies no stage of execution when specified in the first scope.
		// See https://registry.khronos.org/vulkan/specs/latest/man/html/VkPipelineStageFlagBits.html.
		TOP = 1,

		TRANSFER = 2,
		VERTEX_SHADER = 3,
		FRAGMENT_SHADER = 4,
		COMPUTE_SHADER = 5,
	};
}