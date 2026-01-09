// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/API/Rendering/Queue.hpp"

namespace ProjectA {
	class GL::Queue final : public ProjectA::Queue {
		public:
			explicit Queue(const GL::Instance& instance, const GL::Device& device);

			ref<ProjectA::CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(const sequence<ref<ProjectA::CommandBuffer>>& commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<ProjectA::Fence> fence) const final override;

			PresentStatus present(const sequence<ref<ProjectA::Swapchain>>& swapchains, ref<Semaphore> waitSemaphore, const sequence<uint32>& imageIndices) const final override;

			void waitIdle() const final override;

		private:
			const GL::Instance& instance;
			const GL::Device& device;
	};
}