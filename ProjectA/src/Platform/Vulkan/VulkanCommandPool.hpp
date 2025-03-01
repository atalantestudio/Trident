#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/CommandPool.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"

namespace ProjectA {
	class VulkanCommandPool : public CommandPool {
		public:
			VulkanCommandPool(const VulkanPhysicalDevice& physicalDevice, const VkDevice device);

			~VulkanCommandPool();

		public:
			VkCommandPool object;

		private:
			const VkDevice device;
	};
}