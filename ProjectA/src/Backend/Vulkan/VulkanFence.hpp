#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Synchronization/Fence.hpp"

namespace ProjectA {
	class VulkanFence : public Fence, public ObjectWrapper<VkFence> {
		public:
			VulkanFence(VkDevice device);

			~VulkanFence();

			bool isSignaled() const final override;

		private:
			const VkDevice device;
	};
}