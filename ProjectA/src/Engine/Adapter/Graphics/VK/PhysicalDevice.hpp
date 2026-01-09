// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

// #define DEBUG_ENUMERATE_DEVICE_EXTENSIONS

namespace ProjectA {
	class VK::PhysicalDevice final : public ProjectA::PhysicalDevice {
		private:
			static void enumerateQueueFamilyProperties(const sequence<VkQueueFamilyProperties>& queueFamilies);

			static uint32 calculateGraphicsQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties);

			static uint32 calculateComputeQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties);

			static uint32 calculateTransferQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties);

		public:
			explicit PhysicalDevice(VkPhysicalDevice physicalDevice);

		private:
			void setLimits();

			void selectQueueFamilyIndices();

		public:
			VkPhysicalDevice physicalDevice;
			std::vector<const char8*> enabledExtensionNames;
			uint32 graphicsQueueFamilyIndex;
			uint32 computeQueueFamilyIndex;
			uint32 transferQueueFamilyIndex;
			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
	};
}