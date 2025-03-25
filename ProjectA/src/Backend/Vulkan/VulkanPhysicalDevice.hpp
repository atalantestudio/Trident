#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"

namespace ProjectA {
	class VulkanPhysicalDevice : public ObjectWrapper<VkPhysicalDevice> {
		public:
			uint32 graphicsQueueFamilyIndex = 0;
			chain<const char*> enabledExtensionNames;
			VkPhysicalDeviceFeatures features;

		public:
			VulkanPhysicalDevice(VkInstance instance);

		private:
			static uint32 calculateScore(VkPhysicalDevice physicalDevice);
	};
}