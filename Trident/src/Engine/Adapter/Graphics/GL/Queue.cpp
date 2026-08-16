// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/GL/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Queue.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Queue::Queue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) :
		Trident::Queue(device, queueFamilyIndex, queueFamilyCapability)
	{}

	ref<Trident::Queue> GL::createQueue(const Trident::Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Queue> queue = allocateRef<GL::Queue>(device, queueFamilyIndex, queueFamilyCapability);

		return queue;
	}

	void GL::destroyQueue(const Trident::Queue* queue) {}

	PresentStatus GL::present(const Trident::Queue* queue, view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Trident::Semaphore> waitSemaphore) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ADAPTER_ASSERT(swapchains.count() == presentableImageIndices.count(), "There must be 1 presentable image index per swapchain.");

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			static const GL::PFNWGLGETCURRENTDCPROC wglGetCurrentDC = loadWglProcedure<GL::PFNWGLGETCURRENTDCPROC>("wglGetCurrentDC");

			BOOL result;

			static HDC deviceContext = wglGetCurrentDC();

			WIN32_ASSERT(deviceContext != NULL);

			result = SwapBuffers(deviceContext);

			WIN32_ASSERT(result == TRUE);

			return PresentStatus::SUCCESS;
		#else
			return PresentStatus::ERROR_UNHANDLED;
		#endif
	}

	void GL::submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if defined(DEBUG_GL_ENABLE_COMMAND_QUEUE)
			for (const ref<Trident::CommandBuffer>& commandBuffer : commandBuffers) {
				const ref<GL::CommandBuffer> glCommandBuffer = ref_cast<GL::CommandBuffer>(commandBuffer);

				for (ref<GL::Command> command : glCommandBuffer->commands) {
					command->execute();
				}
			}
		#endif
	}

	void GL::submit(const Trident::Queue* queue, view<ref<Trident::CommandBuffer>> commandBuffers, ref<Trident::Semaphore> waitSemaphore, ref<Trident::Semaphore> signalSemaphore, ref<Trident::Fence> fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if defined(DEBUG_GL_ENABLE_COMMAND_QUEUE)
			for (const ref<Trident::CommandBuffer>& commandBuffer : commandBuffers) {
				const ref<GL::CommandBuffer> glCommandBuffer = ref_cast<GL::CommandBuffer>(commandBuffer);

				for (ref<GL::Command> command : glCommandBuffer->commands) {
					command->execute();
				}
			}
		#endif

		DEBUG_ASSERT(fence != nullptr);

		queue->device->waitForFence(fence);
	}

	void GL::waitIdle(const Trident::Queue* queue) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glFinish();
	}
}