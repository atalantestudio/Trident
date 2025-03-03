#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/CommandPool.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	class VulkanCommandPool : public CommandPool, public ObjectWrapper<VkCommandPool> {
		public:
			VulkanCommandPool(const VulkanPhysicalDevice& physicalDevice, const VkDevice device);

			~VulkanCommandPool();

		private:
			const VkDevice device;
	};
}