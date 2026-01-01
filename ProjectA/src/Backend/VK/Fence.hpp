#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Fence.hpp"

namespace ProjectA {
	class VulkanFence final : public Fence, public ObjectWrapper<VkFence> {
		public:
			explicit VulkanFence(VkDevice device);

			~VulkanFence() final override;

		private:
			const VkDevice device;
	};
}