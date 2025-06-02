#include "pch.hpp"

#include "Backend/Vulkan/VulkanCommandBuffer.hpp"

namespace ProjectA {
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount) :
		CommandBuffer(commandCount),
		ObjectWrapper(commandBuffer),
		pipelineStageFlags(pipelineStageFlags)
	{}
}