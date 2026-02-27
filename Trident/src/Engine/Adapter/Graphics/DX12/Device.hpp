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
		AcquireNextImageStatus acquireNextImage(const Trident::Device* device, const Trident::Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex);

		sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(Trident::Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts);

		ref<Trident::Device> createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		void destroyDevice(const Trident::Device* device);

		ref<Trident::Queue> getQueue(const Trident::Device* device, QueueFamilyCapability queueFamilyCapability);

		void resetFence(const Trident::Device* device, const Trident::Fence* fence);

		void updateDescriptorSets(const Trident::Device* device, view<WriteDescriptorSet> writeDescriptorSets);

		void waitForFence(const Trident::Device* device, const Trident::Fence* fence);

		void waitIdle(const Trident::Device* device);
	}
}