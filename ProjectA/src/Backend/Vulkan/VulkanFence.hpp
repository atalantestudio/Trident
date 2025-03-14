#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Synchronization/Fence.hpp"

namespace ProjectA {
	class VulkanFence : public Fence, public ObjectWrapper<VkFence> {
		public:
			VulkanFence(const VkDevice device);

			~VulkanFence();

		private:
			const VkDevice device;
	};
}