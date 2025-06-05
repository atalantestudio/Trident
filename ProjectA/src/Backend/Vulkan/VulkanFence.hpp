#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Fence.hpp"

namespace ProjectA {
	class VulkanFence : public Fence, public ObjectWrapper<VkFence> {
		public:
			VulkanFence(VkDevice device);

			~VulkanFence();

		private:
			const VkDevice device;
	};
}