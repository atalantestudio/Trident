#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"

namespace ProjectA {
	class VulkanPhysicalDevice : public ObjectWrapper<VkPhysicalDevice> {
		public:
			// TODO: lyah::max<uint32>()
			// TODO: lyah::min<uint32>()
			uint32 graphicsQueueFamilyIndex = std::numeric_limits<uint32>::max();
			uint32 computeQueueFamilyIndex = std::numeric_limits<uint32>::max();
			chain<const char*> enabledExtensionNames;
			VkPhysicalDeviceFeatures features;

		public:
			VulkanPhysicalDevice(VkInstance instance);

		private:
			static uint32 calculateScore(VkPhysicalDevice physicalDevice);
	};
}