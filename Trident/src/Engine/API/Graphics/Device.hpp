// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/DescriptorSet.hpp"
#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/Queue.hpp"
#include "Engine/API/Graphics/QueueFamilyCapability.hpp"
#include "Engine/API/Graphics/Status.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"

namespace Trident {
	class Device {
		friend class GraphicsAdapter;

		public:
			~Device();

			/// Acquires the next available swapchain image.
			AcquireNextImageStatus acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts);

			/// Creates and returns a buffer which is pre-allocated from dynamic memory.
			/// For most resources (excluding render pass attachments and other resources depending on the screen size),
			/// prefer Device::createStaticBuffer over this method.
			ref<Buffer> createDynamicBuffer(const BufferDescriptor& descriptor) const;

			/// Creates and returns a buffer which needs to be manually allocated.
			ref<Buffer> createStaticBuffer(const BufferDescriptor& descriptor) const;

			// TODO(graphics/device/create-compute-pipeline): Create multiple pipelines at once.
			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const;

			ref<DescriptorSetLayout> createDescriptorSetLayout(view<DescriptorSetLayoutBinding> bindings) const;

			ref<Fence> createFence(const FenceDescriptor& descriptor) const;

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const;

			// TODO(graphics/device/create-graphics-pipeline): Create multiple pipelines at once.
			ref<GraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) const;

			/// Creates and returns an image which is pre-allocated from dynamic memory.
			/// For most resources (excluding render pass attachments and other resources depending on the screen size),
			/// prefer Device::createStaticImage over this method.
			ref<Image> createDynamicImage(const ImageDescriptor& descriptor) const;

			/// Creates and returns an image which needs to be manually allocated.
			ref<Image> createStaticImage(const ImageDescriptor& descriptor) const;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const;

			ref<RenderPass> createRenderPass(ref<Surface> surface, const RenderPassDescriptor& descriptor) const;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore() const;

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const;

			// TODO: Deprecate (replace with the asset overload above).
			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const;

			ref<MemoryAllocator> getMemoryAllocator() const;

			ref<PhysicalDevice> getPhysicalDevice() const;

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const;

			void resetFence(ref<Fence> fence) const;

			void updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const;

			void waitForFence(ref<Fence> fence) const;

			/// Waits until all queues created with the device reach an idle state.
			/// To wait on a particular queue, see Queue::waitIdle.
			void waitIdle() const;

		protected:
			explicit Device(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice);

		public:
			const GraphicsAdapter* adapter;
			const PhysicalDevice* physicalDevice;

			// The memory allocator is automatically created by the adapter after the API creation of the device.
			ref<MemoryAllocator> memoryAllocator;
	};
}