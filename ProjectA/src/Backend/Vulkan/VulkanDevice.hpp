#pragma once

#include "Backend/Vulkan/VulkanPhysicalDevice.hpp"
#include "Backend/Vulkan/VulkanQueue.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class VulkanDevice final : public Device, public ObjectWrapper<VkDevice> {
		public:
			explicit VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() final override;

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void waitForFence(const ref<Fence>& fence) const final override;

			void resetFence(const ref<Fence>& fence) const final override;

			AcquireNextImageStatus acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const final override;

			void waitIdle() const final override;

		private:
			static const std::vector<const char*> enabledExtensionNames;

			VkPhysicalDeviceMemoryProperties memoryProperties;

		private:
			static constexpr uint32 GRAPHICS_QUEUE_INDEX = 0;
			const ref<VulkanPhysicalDevice> physicalDevice;

			std::array<uint32, 3> queueIndices;
			std::array<ref<VulkanQueue>, 3> queues;
	};
}