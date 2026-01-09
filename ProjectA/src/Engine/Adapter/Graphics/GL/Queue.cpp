// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/GL/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Queue.hpp"

namespace ProjectA {
	GL::Queue::Queue(const GL::Instance& instance, const GL::Device& device) :
		instance(instance),
		device(device)
	{}

	ref<CommandEncoder> GL::Queue::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<GL::CommandEncoder>(descriptor, device);
	}

	void GL::Queue::submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<ProjectA::Fence> fence) const {
		for (const ref<ProjectA::CommandBuffer> commandBuffer : commandBuffers) {
			const ref<GL::CommandBuffer> glCommandBuffer = std::static_pointer_cast<GL::CommandBuffer>(commandBuffer);

			for (const ref<GL::Command> command : glCommandBuffer->commands) {
				command->execute();
			}
		}

		if (fence != nullptr) {
			device.waitForFence(fence);
		}
	}

	PresentStatus GL::Queue::present(const sequence<ref<ProjectA::Swapchain>>& swapchains, ref<Semaphore> waitSemaphore, const sequence<uint32>& imageIndices) const {
		instance.swapBuffers();

		return PresentStatus::SUCCESS;
	}

	void GL::Queue::waitIdle() const {
		glFinish();
	}
}