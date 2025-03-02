#pragma once

#include <vulkan/vulkan.h>

namespace ProjectA {
	class VulkanFence {
		public:
			VulkanFence(const VkDevice device);

			~VulkanFence();

		public:
			VkFence object;

		private:
			const VkDevice device;
	};
}