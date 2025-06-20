#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace ProjectA {
	class VulkanPhysicalDevice final : public PhysicalDevice, public ObjectWrapper<VkPhysicalDevice> {
		public:
			explicit VulkanPhysicalDevice(VkInstance instance);

		private:
			void setLimits();

			void selectQueueFamilyIndices();

			static void enumerateQueueFamilyProperties(const sequence<VkQueueFamilyProperties>& queueFamilies);

			static uint32 calculatePhysicalDeviceScore(const VkPhysicalDeviceProperties& physicalDeviceProperties);

			static uint32 calculateGraphicsQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties);

			static uint32 calculateComputeQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties);

			static uint32 calculateTransferQueueFamilyScore(const VkQueueFamilyProperties& queueFamilyProperties);

		public:
			std::vector<const char*> enabledExtensionNames;

			uint32 graphicsQueueFamilyIndex;
			uint32 computeQueueFamilyIndex;
			uint32 transferQueueFamilyIndex;

			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;
			VkPhysicalDeviceProperties properties;
	};
}