#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/CommandPool.hpp"

namespace ProjectA {
	class VulkanCommandPool : public CommandPool, public ObjectWrapper<VkCommandPool> {
		public:
			VulkanCommandPool(VkDevice device, uint32 queueFamilyIndex);

			~VulkanCommandPool();

		private:
			const VkDevice device;
	};
}