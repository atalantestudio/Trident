#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/CommandBuffer.hpp"

namespace ProjectA {
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount) :
		CommandBuffer(commandCount),
		ObjectWrapper(commandBuffer),
		pipelineStageFlags(pipelineStageFlags)
	{}
}