#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Device.hpp"

#include "Platform/Vulkan/VulkanFence.hpp"
#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanSemaphore.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanDevice : public Device, public ObjectWrapper<VkDevice> {
		public:
			VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() override;

			// Returns the most suitable device memory type index for the allocated buffer.
			uint32 allocateMemory(const VkMemoryRequirements& memoryRequirements);

			VkDeviceMemory getMemory(const uint32 memoryTypeIndex) const;

			void writeBuffer(const void* data, const uint64 size, const uint32 memoryTypeIndex) const;

			void waitForFences() const override;

			void resetFences() const override;

			uint32 acquireNextImage(const ref<Swapchain>& swapchain) const override;

			void submit(const ref<CommandBuffer>& commandBuffer) const override;

			void present(const ref<Swapchain>& swapchain, const uint32 imageIndex) const override;

			void waitIdle() const override;

		private:
			static const std::vector<const char*> enabledExtensionNames;

		private:
			const VkPhysicalDevice physicalDevice;
			VkQueue graphicsQueue;
			VkQueue presentQueue;
			ref<VulkanSemaphore> acquireNextImageSemaphore;
			ref<VulkanSemaphore> signalSemaphore;
			ref<VulkanFence> flightFence;

			// TODO: Replace with sparse set implementation?
			std::vector<VkDeviceMemory> memory;
			std::vector<uint32> memoryTypeIndexToMemoryIndex;
	};
}