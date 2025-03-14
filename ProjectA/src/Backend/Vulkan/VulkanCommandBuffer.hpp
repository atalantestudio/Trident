#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"

namespace ProjectA {
	class VulkanCommandBuffer : public CommandBuffer, public ObjectWrapper<const VkCommandBuffer> {
		public:
			VulkanCommandBuffer(const VkCommandBuffer commandBuffer);
	};
}