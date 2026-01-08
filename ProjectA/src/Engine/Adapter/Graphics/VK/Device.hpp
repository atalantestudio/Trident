#pragma once

#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/VK/Queue.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class VulkanDevice final : public Device, public ObjectWrapper<VkDevice> {
		public:
			explicit VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface);

			~VulkanDevice() final override;

			AcquireNextImageStatus acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<Fence> fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(ref<Fence> fence) const final override;

			void waitIdle() const final override;

		private:
			static const std::vector<const char8*> enabledExtensionNames;

		private:
			const VulkanPhysicalDevice& physicalDevice;

			VkPhysicalDeviceMemoryProperties memoryProperties;
			std::vector<VkDescriptorPool> descriptorPools;
			std::array<ref<VulkanQueue>, 3> queues;
	};
}