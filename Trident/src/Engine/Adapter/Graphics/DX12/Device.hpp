// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"
#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class DX12::Device final : public Trident::Device {
		public:
			explicit Device(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

			sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts) final override;

			ref<Trident::Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const final override;

		public:
			IDXGIFactory4* factory;
			ID3D12Device* device;
			ref<Trident::Queue> queues[3];
			HANDLE waitEvent;

			#if defined(DEBUG_ENABLE_MESSENGER)
				ID3D12InfoQueue1* infoQueue;
				DWORD messageCallbackCookie;
			#endif
	};

	namespace DX12 {
		ref<Trident::Device> createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		void destroyDevice(const Trident::Device* device);

		AcquireNextImageStatus acquireNextImage(const Trident::Device* device, const Trident::Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex);

		void resetFence(const Trident::Device* device, const Trident::Fence* fence);

		void waitForFence(const Trident::Device* device, const Trident::Fence* fence);

		void waitIdle(const Trident::Device* device);
	}
}