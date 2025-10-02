#pragma once

#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorSetLayout.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"
#include "Engine/API/Rendering/Queue.hpp"
#include "Engine/API/Rendering/QueueFamilyCapability.hpp"
#include "Engine/API/Rendering/Status.hpp"

namespace ProjectA {
	class Device {
		public:
			virtual ~Device() = default;

			// Acquires the next available swapchain image and writes its index to imageIndex.
			virtual AcquireNextImageStatus acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const = 0;

			virtual sequence<ref<DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) = 0;

			virtual ref<PhysicalDevice> getPhysicalDevice() const = 0;

			virtual ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const = 0;

			virtual void resetFence(const ref<Fence>& fence) const = 0;

			virtual void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const = 0;

			virtual void waitForFence(const ref<Fence>& fence) const = 0;

			virtual void waitIdle() const = 0;
	};
}