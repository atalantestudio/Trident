#include "pch.hpp"

#include "Backend/Vulkan/VulkanCommandBuffer.hpp"

namespace ProjectA {
	VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags) :
		ObjectWrapper(commandBuffer),
		pipelineStageFlags(pipelineStageFlags)
	{}
}