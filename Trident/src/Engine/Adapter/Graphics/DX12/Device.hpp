// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class DX12::Device final : public Trident::Device {
		private:
			#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
				static void messageCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR description, void* context);
			#endif

		public:
			explicit Device(const DX12::Instance& instance, const DX12::PhysicalDevice& physicalDevice);

			~Device() final override;

			AcquireNextImageStatus acquireNextImage(ref<Trident::Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<Trident::PhysicalDevice> getPhysicalDevice() const final override;

			ref<Trident::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<Trident::Fence> fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(ref<Trident::Fence> fence) const final override;

			void waitIdle() const final override;

			void setMemoryAllocator(ref<DX12::MemoryAllocator> memoryAllocator);

		public:
			ComPtr<ID3D12Device> device;
			ref<DX12::MemoryAllocator> memoryAllocator;

		private:
			ref<Trident::Queue> queues[3];
			HANDLE waitEvent;
			const DX12::PhysicalDevice& physicalDevice;

			#if defined(DEBUG_ENABLE_VALIDATION_LAYERS)
				ID3D12InfoQueue1* infoQueue;
				DWORD messageCallbackCookie;
			#endif
	};
}