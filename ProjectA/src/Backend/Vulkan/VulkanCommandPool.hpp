#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/CommandPool.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	class VulkanCommandPool : public CommandPool, public ObjectWrapper<VkCommandPool> {
		public:
			VulkanCommandPool(const VulkanPhysicalDevice& physicalDevice, VkDevice device);

			~VulkanCommandPool();

		private:
			const VkDevice device;
	};
}