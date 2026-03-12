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
			explicit Device(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		public:
			ref<Trident::Queue> queue;
	};

	namespace GL {
		AcquireNextImageStatus acquireNextImage(const Trident::Device* device, const Trident::Swapchain* swapchain, const Trident::Semaphore* semaphore, uint32& imageIndex);

		sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(Trident::Device* device, view<ref<Trident::DescriptorSetLayout>> descriptorSetLayouts);

		ref<Trident::Device> createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		void destroyDevice(const Trident::Device* device);

		ref<Trident::Queue> getQueue(const Trident::Device* device, QueueFamilyCapability queueFamilyCapability);

		void resetFence(const Trident::Device* device, const Trident::Fence* fence);

		void updateDescriptorSets(const Trident::Device* device, view<WriteDescriptorSet> writeDescriptorSets);

		void waitForFence(const Trident::Device* device, Trident::Fence* fence);

		void waitIdle(const Trident::Device* device);
	}
}