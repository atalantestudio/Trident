#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Device.hpp"

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanDevice : public Device, public ObjectWrapper<VkDevice> {
		public:
			VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() override;

			// Returns a suitable device memory type index.
			uint32 findMemoryType(const VkMemoryRequirements& memoryRequirements, const VkMemoryPropertyFlags memoryPropertyFlags) const;

			void waitForFence(const ref<Fence>& fence) const override;

			void resetFence(const ref<Fence>& fence) const override;

			Status acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& nextImageIndex) const override;

			void submit(const ref<CommandBuffer>& commandBuffer, const ref<Semaphore>& waitSemaphore, const ref<Semaphore>& signalSemaphore, const ref<Fence>& fence) const override;

			Status present(const ref<Swapchain>& swapchain, const ref<Semaphore>& waitSemaphore, const uint32 imageIndex) const override;

			void waitIdle() const override;

		private:
			static const chain<const char*> enabledExtensionNames;

		private:
			const VkPhysicalDevice physicalDevice;
			VkQueue presentQueue;

		public:
			VkQueue graphicsQueue;
	};
}