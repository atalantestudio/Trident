// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/CommandBuffer.hpp"

namespace Trident {
	VK::CommandBuffer::CommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount) :
		Trident::CommandBuffer(commandCount),
		commandBuffer(commandBuffer),
		pipelineStageFlags(pipelineStageFlags)
	{}
}