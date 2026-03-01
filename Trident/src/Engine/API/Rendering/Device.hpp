// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/DescriptorSetLayout.hpp"
#include "Engine/API/Rendering/Queue.hpp"
#include "Engine/API/Rendering/QueueFamilyCapability.hpp"
#include "Engine/API/Rendering/Status.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class Device {
		friend class GraphicsAdapter;

		public:
			~Device() {
				adapter->destroyDevice(this);
			}

			// Acquires the next available swapchain image and writes its index to imageIndex.
			AcquireNextImageStatus acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const {
				return adapter->acquireNextImage(this, swapchain.get(), semaphore.get(), imageIndex);
			}

			sequence<ref<DescriptorSet>> allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts) {
				return adapter->allocateDescriptorSets(this, descriptorSetLayouts);
			}

			template<typename ElementType>
			ref<Buffer<ElementType>> createBuffer(const BufferDescriptor& descriptor) const {
				return adapter->createBuffer<ElementType>(this, descriptor);
			}

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
				return adapter->createComputePipeline(this, descriptor);
			}

			// TODO: Use view<DescriptorSetLayoutBinding> instead.
			ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
				return adapter->createDescriptorSetLayout(this, bindings);
			}

			ref<Fence> createFence(const FenceDescriptor& descriptor) const {
				return adapter->createFence(this, descriptor);
			}

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const {
				return adapter->createFramebuffer(this, descriptor);
			}

			ref<GraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) const {
				return adapter->createGraphicsPipeline(this, descriptor);
			}

			ref<Image> createImage(const ImageDescriptor& descriptor) const {
				return adapter->createImage(this, descriptor);
			}

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
				return adapter->createPipelineLayout(this, descriptor);
			}

			ref<RenderPass> createRenderPass(ref<Surface> surface, const RenderPassDescriptor& descriptor) const {
				return adapter->createRenderPass(this, surface, descriptor);
			}

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const {
				return adapter->createSampler(this, descriptor);
			}

			ref<Semaphore> createSemaphore() const {
				return adapter->createSemaphore(this);
			}

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
				return adapter->createShaderModule(this, descriptor, asset);
			}

			// TODO: Deprecate (replace with the asset overload above).
			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const {
				return adapter->createShaderModule(this, descriptor, code);
			}

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const {
				return adapter->createSwapchain(this, descriptor);
			}

			ref<MemoryAllocator> getMemoryAllocator() const {
				return memoryAllocator;
			}

			ref<PhysicalDevice> getPhysicalDevice() const {
				return createRef<PhysicalDevice>(*physicalDevice);
			}

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const {
				return adapter->getQueue(this, queueFamilyCapability);
			}

			void resetFence(ref<Fence> fence) const {
				adapter->resetFence(this, fence.get());
			}

			void updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const {
				adapter->updateDescriptorSets(this, writeDescriptorSets);
			}

			void waitForFence(ref<Fence> fence) const {
				adapter->waitForFence(this, fence.get());
			}

			// Waits until all queues created with the device reach an idle state.
			// To wait on a particular queue, see `Queue::waitIdle`.
			void waitIdle() const {
				adapter->waitIdle(this);
			}

		protected:
			explicit Device(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice) :
				adapter(adapter),
				physicalDevice(physicalDevice)
			{}

		public:
			const GraphicsAdapter* adapter;
			const PhysicalDevice* physicalDevice;

			// The memory allocator is automatically created by the adapter after the API creation of the device.
			ref<MemoryAllocator> memoryAllocator;
	};
}