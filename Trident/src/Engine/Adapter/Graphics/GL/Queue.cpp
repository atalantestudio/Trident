// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/GL/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Queue.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Queue::Queue(const GL::Instance& instance, const GL::Device& device) :
		instance(instance),
		device(device)
	{}

	ref<CommandEncoder> GL::Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<GL::CommandEncoder>(descriptor, device);
	}

	void GL::Queue::submit(view<ref<Trident::CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Trident::Fence> fence) const {
		for (const ref<Trident::CommandBuffer> commandBuffer : commandBuffers) {
			const ref<GL::CommandBuffer> glCommandBuffer = std::static_pointer_cast<GL::CommandBuffer>(commandBuffer);

			for (const ref<GL::Command> command : glCommandBuffer->commands) {
				command->execute();
			}
		}

		if (fence != nullptr) {
			device.waitForFence(fence);
		}
	}

	PresentStatus GL::Queue::present(view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const {
		ADAPTER_ASSERT(swapchains.count() == presentableImageIndices.count(), "There must be 1 presentable image index per swapchain.");

		instance.swapBuffers();

		return PresentStatus::SUCCESS;
	}

	void GL::Queue::waitIdle() const {
		glFinish();
	}
}