// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/CommandBuffer.hpp"

namespace ProjectA {
	class VK::CommandBuffer final : public ProjectA::CommandBuffer {
		public:
			explicit CommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount);

		public:
			VkCommandBuffer commandBuffer;
			const VkPipelineStageFlags pipelineStageFlags;
	};
}