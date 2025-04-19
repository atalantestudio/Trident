#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"

namespace ProjectA {
	class VulkanSemaphore : public Semaphore, public ObjectWrapper<VkSemaphore> {
		public:
			VulkanSemaphore(VkDevice device);

			~VulkanSemaphore();

		private:
			const VkDevice device;
	};
}