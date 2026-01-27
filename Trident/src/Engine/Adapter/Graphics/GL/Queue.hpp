// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/API/Rendering/Queue.hpp"

namespace Trident {
	class GL::Queue final : public Trident::Queue {
		public:
			explicit Queue(const GL::Instance& instance, const GL::Device& device);

			ref<Trident::CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const final override;

			void submit(view<ref<Trident::CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Trident::Fence> fence) const final override;

			PresentStatus present(view<ref<Trident::Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const final override;

			void waitIdle() const final override;

		private:
			const GL::Instance& instance;
			const GL::Device& device;
	};
}