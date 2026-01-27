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

			void submit(const sequence<ref<Trident::CommandBuffer>>& commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Trident::Fence> fence) const final override;

			PresentStatus present(const sequence<ref<Trident::Swapchain>>& swapchains, const sequence<uint32>& imageIndices, ref<Semaphore> waitSemaphore) const final override;

			void waitIdle() const final override;

		private:
			const GL::Instance& instance;
			const GL::Device& device;
	};
}