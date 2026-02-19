// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorSetLayout.hpp"
#include "Engine/API/Rendering/Queue.hpp"
#include "Engine/API/Rendering/QueueFamilyCapability.hpp"
#include "Engine/API/Rendering/Status.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class PhysicalDevice;

	class Device {
		public:
			virtual ~Device() = default;

			// Acquires the next available swapchain image and writes its index to imageIndex.
			virtual AcquireNextImageStatus acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const = 0;

			virtual sequence<ref<DescriptorSet>> allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts) = 0;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const {
				return adapter->createSwapchain(descriptor, this);
			}

			virtual ref<PhysicalDevice> getPhysicalDevice() const = 0;

			virtual ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const = 0;

			virtual void resetFence(ref<Fence> fence) const = 0;

			virtual void updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const = 0;

			virtual void waitForFence(ref<Fence> fence) const = 0;

			// Waits until all queues created with the device reach an idle state.
			// To wait on a particular queue, see `Queue::waitIdle`.
			virtual void waitIdle() const = 0;

		protected:
			explicit Device(const GraphicsAdapter* adapter) :
				adapter(adapter)
			{}

		private:
			const GraphicsAdapter* adapter;
	};
}