#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/PhysicalDevice.hpp"

namespace ProjectA {
	class VulkanPhysicalDevice : public PhysicalDevice, public ObjectWrapper<VkPhysicalDevice> {
		public:
			VulkanPhysicalDevice(VkInstance instance);

		private:
			static uint32 calculateScore(VkPhysicalDevice physicalDevice);

			void setLimits();

		public:
			// TODO: lyah::max<uint32>()
			// TODO: lyah::min<uint32>()
			uint32 graphicsQueueFamilyIndex = std::numeric_limits<uint32>::max();
			uint32 computeQueueFamilyIndex = std::numeric_limits<uint32>::max();
			std::vector<const char*> enabledExtensionNames;

			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
	};
}