#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/PhysicalDevice.hpp"

namespace ProjectA {
	class VulkanPhysicalDevice : public PhysicalDevice, public ObjectWrapper<VkPhysicalDevice> {
		public:
			VulkanPhysicalDevice(VkInstance instance);

			const PhysicalDeviceLimits& getLimits() const override;

		private:
			static uint32 calculateScore(VkPhysicalDevice physicalDevice);

		public:
			// TODO: lyah::max<uint32>()
			// TODO: lyah::min<uint32>()
			uint32 graphicsQueueFamilyIndex = std::numeric_limits<uint32>::max();
			uint32 computeQueueFamilyIndex = std::numeric_limits<uint32>::max();
			chain<const char*> enabledExtensionNames;
			VkPhysicalDeviceFeatures features;
	};
}