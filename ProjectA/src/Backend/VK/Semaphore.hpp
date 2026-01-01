#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Semaphore.hpp"

namespace ProjectA {
	class VulkanSemaphore final : public Semaphore, public ObjectWrapper<VkSemaphore> {
		public:
			explicit VulkanSemaphore(VkDevice device);

			~VulkanSemaphore() final override;

		private:
			const VkDevice device;
	};
}