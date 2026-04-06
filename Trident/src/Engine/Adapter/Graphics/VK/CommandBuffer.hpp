// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandBuffer.hpp"

namespace Trident {
	class VK::CommandBuffer final : public Trident::CommandBuffer {
		public:
			explicit CommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount);

		public:
			VkCommandBuffer commandBuffer;
			const VkPipelineStageFlags pipelineStageFlags;
	};
}