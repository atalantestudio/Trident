#include "pch.hpp"

#include "Backend/Vulkan/VulkanCommandBuffer.hpp"

namespace ProjectA {
	VulkanCommandBuffer::VulkanCommandBuffer(const VkCommandBuffer commandBuffer) :
		ObjectWrapper(commandBuffer)
	{}
}