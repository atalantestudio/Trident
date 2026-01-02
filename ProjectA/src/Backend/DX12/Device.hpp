// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Backend/DX12/Instance.hpp"

#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class DX12::Device final : public ProjectA::Device {
		public:
			explicit Device(const DX12::Instance& instance);

			~Device();

			AcquireNextImageStatus acquireNextImage(const ref<ProjectA::Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const final override;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<ProjectA::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(const ref<Fence>& fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(const ref<Fence>& fence) const final override;

			void waitIdle() const final override;

		public:
			ComPtr<ID3D12Device> device;

		private:
			std::array<ref<ProjectA::Queue>, 3> queues;
	};
}