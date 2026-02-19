// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class VK::Device final : public Trident::Device {
		public:
			explicit Device(const VK::PhysicalDevice& physicalDevice);

			~Device() final override;

			AcquireNextImageStatus acquireNextImage(ref<Trident::Swapchain> swapchain, ref<Trident::Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(view<ref<Trident::DescriptorSetLayout>> descriptorSetLayouts) final override;

			ref<Trident::PhysicalDevice> getPhysicalDevice() const final override;

			ref<Trident::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<Trident::Fence> fence) const final override;

			void updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const final override;

			void waitForFence(ref<Trident::Fence> fence) const final override;

			void waitIdle() const final override;

		private:
			static const sequence<const char8*> ENABLED_EXTENSION_NAMES;

		public:
			VkDevice device;

		private:
			const VK::PhysicalDevice& physicalDevice;
			VkPhysicalDeviceMemoryProperties memoryProperties;
			std::vector<VkDescriptorPool> descriptorPools;
			ref<VK::Queue> queues[3];
	};
}