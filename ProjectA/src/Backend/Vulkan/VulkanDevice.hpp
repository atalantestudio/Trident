#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Device.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"
#include "Backend/Vulkan/VulkanQueue.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanDevice : public Device, public ObjectWrapper<VkDevice> {
		public:
			VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() override;

			// Returns a suitable device memory type index.
			uint32 findMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags memoryPropertyFlags) const;

			ref<Queue> getComputeQueue() const override;

			ref<Queue> getGraphicsQueue() const override;

			void waitForFence(const ref<Fence>& fence) const override;

			void resetFence(const ref<Fence>& fence) const override;

			Status acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const override;

			Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, uint32 imageIndex) const override;

			void waitIdle() const override;

		private:
			static const chain<const char*> enabledExtensionNames;

		private:
			const VkPhysicalDevice physicalDevice;
			ref<VulkanQueue> presentQueue;

		public:
			ref<VulkanQueue> graphicsQueue;
			ref<VulkanQueue> computeQueue;
	};
}