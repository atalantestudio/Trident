// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/PhysicalDevice.hpp"

namespace Trident {
	class VK::PhysicalDevice final : public Trident::PhysicalDevice {
		public:
			explicit PhysicalDevice(const Trident::GraphicsAdapter* adapter);

		public:
			const VK::Instance* instance;
			VkPhysicalDevice physicalDevice;
			uint32 graphicsQueueFamilyIndex;
			uint32 computeQueueFamilyIndex;
			uint32 transferQueueFamilyIndex;
			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
	};

	namespace VK {
		ref<Trident::PhysicalDevice> selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu);

		uint32 getPhysicalDeviceMemoryTypeCountImplementation(const Trident::PhysicalDevice* physicalDevice);

		view<char8> getPhysicalDeviceName(const Trident::PhysicalDevice* physicalDevice);
	}
}