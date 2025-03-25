#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Synchronization/Semaphore.hpp"

namespace ProjectA {
	class VulkanSemaphore : public Semaphore, public ObjectWrapper<VkSemaphore> {
		public:
			VulkanSemaphore(VkDevice device);

			~VulkanSemaphore();

		private:
			const VkDevice device;
	};
}