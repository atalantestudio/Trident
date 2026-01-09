// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class VK::Device final : public ProjectA::Device {
		public:
			explicit Device(const VK::PhysicalDevice& physicalDevice, const VK::Surface& surface);

			~Device() final override;

			AcquireNextImageStatus acquireNextImage(ref<ProjectA::Swapchain> swapchain, ref<ProjectA::Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<ProjectA::DescriptorSet>> allocateDescriptorSets(const sequence<ref<ProjectA::DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<ProjectA::PhysicalDevice> getPhysicalDevice() const final override;

			ref<ProjectA::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<ProjectA::Fence> fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(ref<ProjectA::Fence> fence) const final override;

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