// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/GL/Queue.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class GL::Device final : public ProjectA::Device {
		public:
			explicit Device(const GL::Instance& instance);

			AcquireNextImageStatus acquireNextImage(ref<ProjectA::Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<ProjectA::DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<ProjectA::PhysicalDevice> getPhysicalDevice() const final override;

			ref<ProjectA::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<ProjectA::Fence> fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(ref<ProjectA::Fence> fence) const final override;

			void waitIdle() const final override;

		private:
			ref<GL::PhysicalDevice> physicalDevice;
			ref<GL::Queue> queue;
	};
}