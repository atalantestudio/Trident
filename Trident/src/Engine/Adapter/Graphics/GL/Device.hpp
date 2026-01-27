// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/GL/Queue.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class GL::Device final : public Trident::Device {
		public:
			explicit Device(const GL::Instance& instance);

			AcquireNextImageStatus acquireNextImage(ref<Trident::Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<Trident::PhysicalDevice> getPhysicalDevice() const final override;

			ref<Trident::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<Trident::Fence> fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(ref<Trident::Fence> fence) const final override;

			void waitIdle() const final override;

		private:
			ref<GL::PhysicalDevice> physicalDevice;
			ref<GL::Queue> queue;
	};
}