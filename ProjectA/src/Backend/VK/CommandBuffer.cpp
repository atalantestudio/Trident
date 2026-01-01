#include "pch.hpp"

#include "Backend/VK/CommandBuffer.hpp"

namespace ProjectA {
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount) :
		CommandBuffer(commandCount),
		ObjectWrapper(commandBuffer),
		pipelineStageFlags(pipelineStageFlags)
	{}
}