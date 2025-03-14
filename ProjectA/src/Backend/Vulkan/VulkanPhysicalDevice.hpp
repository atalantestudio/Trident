#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"

namespace ProjectA {
	class VulkanPhysicalDevice : public ObjectWrapper<VkPhysicalDevice> {
		public:
			uint32 graphicsQueueFamilyIndex = 0;
			std::vector<const char*> enabledExtensionNames;
			VkPhysicalDeviceFeatures features;

		public:
			VulkanPhysicalDevice(const VkInstance instance);

		private:
			static uint32 calculateScore(const VkPhysicalDevice physicalDevice);
	};
}