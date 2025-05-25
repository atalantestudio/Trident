#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"
#include "Backend/Vulkan/VulkanQueue.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Device.hpp"

namespace ProjectA {
	class VulkanDevice : public Device, public ObjectWrapper<VkDevice> {
		public:
			VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() final override;

			// Returns a suitable device memory type index.
			uint32 findMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags) const;

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<Queue> getComputeQueue() const final override;

			ref<Queue> getGraphicsQueue() const final override;

			ref<Queue> getPresentQueue() const final override;

			void waitForFence(const ref<Fence>& fence) const final override;

			void resetFence(const ref<Fence>& fence) const final override;

			AcquireNextImageStatus acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const final override;

			void waitIdle() const final override;

		private:
			static const std::vector<const char*> enabledExtensionNames;

			VkPhysicalDeviceMemoryProperties memoryProperties;

		private:
			const ref<VulkanPhysicalDevice> physicalDevice;
			ref<VulkanQueue> graphicsQueue;
			ref<VulkanQueue> computeQueue;
			ref<VulkanQueue> presentQueue;
	};
}