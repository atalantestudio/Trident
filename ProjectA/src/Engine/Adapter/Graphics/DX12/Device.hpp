// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class DX12::Device final : public ProjectA::Device {
		private:
			#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
				static void messageCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* context);
			#endif

		public:
			explicit Device(const DX12::Instance& instance, const DX12::PhysicalDevice& physicalDevice);

			~Device() final override;

			AcquireNextImageStatus acquireNextImage(ref<ProjectA::Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<ProjectA::DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<ProjectA::PhysicalDevice> getPhysicalDevice() const final override;

			ref<ProjectA::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<ProjectA::Fence> fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(ref<ProjectA::Fence> fence) const final override;

			void waitIdle() const final override;

		public:
			ComPtr<ID3D12Device> device;

		private:
			ref<ProjectA::Queue> queues[3];
			HANDLE waitEvent;
			const DX12::PhysicalDevice& physicalDevice;

			#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
				ID3D12InfoQueue1* infoQueue;
				DWORD messageCallbackCookie;
			#endif
	};
}