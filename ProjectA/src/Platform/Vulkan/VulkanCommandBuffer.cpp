#include "pch.hpp"

#include "Platform/Vulkan/VulkanCommandBuffer.hpp"

namespace ProjectA {
	VulkanCommandBuffer::VulkanCommandBuffer(const VkCommandBuffer commandBuffer) :
		ObjectWrapper(commandBuffer)
	{}
}