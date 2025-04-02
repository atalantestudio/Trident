#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"
#include "Backend/Vulkan/VulkanQueue.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Device.hpp"

namespace ProjectA {
	class VulkanDevice : public Device, public ObjectWrapper<VkDevice> {
		public:
			VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() override;

			// Returns a suitable device memory type index.
			uint32 findMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags) const;

			ref<PhysicalDevice> getPhysicalDevice() const override;

			ref<Queue> getComputeQueue() const override;

			ref<Queue> getGraphicsQueue() const override;

			void waitForFence(const ref<Fence>& fence) const override;

			void resetFence(const ref<Fence>& fence) const override;

			Status acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const override;

			Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const override;

			void waitIdle() const override;

		public:
			ref<VulkanQueue> graphicsQueue;
			ref<VulkanQueue> computeQueue;

		private:
			static const chain<const char*> enabledExtensionNames;

			VkPhysicalDeviceMemoryProperties memoryProperties;

		private:
			const ref<VulkanPhysicalDevice> physicalDevice;
			ref<VulkanQueue> presentQueue;
	};
}