// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorSetLayout.hpp"
#include "Engine/API/Rendering/Queue.hpp"
#include "Engine/API/Rendering/QueueFamilyCapability.hpp"
#include "Engine/API/Rendering/Status.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class PhysicalDevice;

	class Device {
		friend class GraphicsAdapter;

		public:
			virtual ~Device() = default;

			ref<MemoryAllocator> getMemoryAllocator() const {
				return memoryAllocator;
			}

			// Acquires the next available swapchain image and writes its index to imageIndex.
			virtual AcquireNextImageStatus acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const = 0;

			virtual sequence<ref<DescriptorSet>> allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts) = 0;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const {
				return adapter->createBuffer(descriptor, this);
			}

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
				return adapter->createComputePipeline(descriptor, this);
			}

			// TODO: Use view<DescriptorSetLayoutBinding> instead.
			ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
				return adapter->createDescriptorSetLayout(bindings, this);
			}

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
				return adapter->createPipelineLayout(descriptor, this);
			}

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
				return adapter->createShaderModule(descriptor, asset, this);
			}

			// TODO: Deprecate (replace with the asset overload above).
			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const {
				return adapter->createShaderModule(descriptor, code, this);
			}

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const {
				return adapter->createSwapchain(descriptor, this);
			}

			virtual ref<PhysicalDevice> getPhysicalDevice() const = 0;

			virtual ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const = 0;

			virtual void resetFence(ref<Fence> fence) const = 0;

			virtual void updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const = 0;

			virtual void waitForFence(ref<Fence> fence) const = 0;

			// Waits until all queues created with the device reach an idle state.
			// To wait on a particular queue, see `Queue::waitIdle`.
			virtual void waitIdle() const = 0;

		protected:
			explicit Device(const GraphicsAdapter* adapter) :
				adapter(adapter)
			{}

		private:
			const GraphicsAdapter* adapter;

			// The memory allocator is automatically created by the adapter after the API creation of the device.
			ref<MemoryAllocator> memoryAllocator;
	};
}