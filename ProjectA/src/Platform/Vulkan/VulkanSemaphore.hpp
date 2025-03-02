#pragma once

#include <vulkan/vulkan.h>

namespace ProjectA {
	class VulkanSemaphore {
		public:
			VulkanSemaphore(const VkDevice device);

			~VulkanSemaphore();

		public:
			VkSemaphore object;

		private:
			const VkDevice device;
	};
}