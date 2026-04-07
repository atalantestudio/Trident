// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Queue.hpp"

namespace Trident {
	Queue::Queue(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) :
		device(device),
		commandPool(device->adapter->createCommandPoolImplementation(device, queueFamilyIndex, queueFamilyCapability))
	{}

	Queue::~Queue() {
		device->adapter->destroyQueueImplementation(this);
	}

	ref<CommandEncoder> Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 warningCount = 0;
			uint32 errorCount = 0;

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this command encoder.");

				warningCount += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		#endif

		return device->adapter->createCommandEncoderImplementation(device, commandPool.get(), descriptor);
	}

	void Queue::submit(view<ref<CommandBuffer>> commandBuffers) const {
		device->adapter->submitImplementation(this, commandBuffers);
	}

	void Queue::submit(view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const {
		device->adapter->submit2Implementation(this, commandBuffers, waitSemaphore, signalSemaphore, fence);
	}

	PresentStatus Queue::present(view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const {
		return device->adapter->presentImplementation(this, swapchains, presentableImageIndices, waitSemaphore);
	}

	void Queue::waitIdle() const {
		device->adapter->queueWaitIdleImplementation(this);
	}
}