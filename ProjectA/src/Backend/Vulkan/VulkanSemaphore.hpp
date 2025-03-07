#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Synchronization/Semaphore.hpp"

namespace ProjectA {
	class VulkanSemaphore : public Semaphore, public ObjectWrapper<VkSemaphore> {
		public:
			VulkanSemaphore(const VkDevice device);

			~VulkanSemaphore();

		private:
			const VkDevice device;
	};
}