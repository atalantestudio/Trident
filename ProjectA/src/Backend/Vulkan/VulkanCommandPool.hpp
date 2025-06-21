#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/CommandPool.hpp"

namespace ProjectA {
	class VulkanCommandPool final : public CommandPool, public ObjectWrapper<VkCommandPool> {
		public:
			explicit VulkanCommandPool(VkDevice device, uint32 queueFamilyIndex);

			~VulkanCommandPool() final override;

		private:
			const VkDevice device;
	};
}