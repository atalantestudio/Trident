#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/Device.hpp"

#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanDevice : public Device {
		public:
			VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() override;

			void allocateMemory(const VkMemoryRequirements& memoryRequirements);

			void writeBuffer(const uint64 size, const void* data) const;

		public:
			VkDevice object;
			VkDeviceMemory memory;

		private:
			static const std::vector<const char*> enabledExtensionNames;

		private:
			const VkPhysicalDevice physicalDevice;
			VkQueue graphicsQueue;
			VkQueue presentQueue;
	};
}