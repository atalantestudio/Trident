// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class GraphicsAdapter {
		public:
			static ConsoleLogger logger;

		public:
			explicit GraphicsAdapter(const ApplicationProperties& properties);

			GraphicsAPI getApi() const;

			AcquireNextImageStatus acquireNextImage(const Device* device, const Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex) const;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(const Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts) const;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor, const Device* device) const;

			template<typename ElementType>
			ref<Buffer2<ElementType>> createBuffer2(const BufferDescriptor& descriptor, const Device* device) const {
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				ref<Buffer> buffer = createBufferImplementation(device, descriptor, static_cast<uint32>(sizeof(ElementType)));

				return std::static_pointer_cast<Buffer2<ElementType>>(buffer);
			}

			ref<CommandEncoder> createCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandBufferDescriptor& descriptor) const;

			ref<CommandPool> createCommandPool(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) const;

			ref<ComputePipeline> createComputePipeline(const Device* device, const ComputePipelineDescriptor& descriptor) const;

			ref<DescriptorSetLayout> createDescriptorSetLayout(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) const;

			ref<Device> createDevice(const PhysicalDevice* physicalDevice) const;

			ref<Fence> createFence(const Device* device, const FenceDescriptor& descriptor) const;

			ref<Framebuffer> createFramebuffer(const Device* device, const FramebufferDescriptor& descriptor) const;

			ref<Image> createImage(const ImageDescriptor& descriptor) const;

			ref<Instance> createInstance(const InstanceDescriptor& descriptor) const;

			ref<PipelineLayout> createPipelineLayout(const Device* device, const PipelineLayoutDescriptor& descriptor) const;

			ref<RenderPass> createRenderPass(const Device* device, const RenderPassDescriptor& descriptor) const;

			ref<GraphicsPipeline> createGraphicsPipeline(const Device* device, const GraphicsPipelineDescriptor& descriptor) const;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore(const Device* device) const;

			ref<ShaderModule> createShaderModule(const Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const;
			ref<ShaderModule> createShaderModule(const Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code) const;

			ref<Surface> createSurface(const Instance* instance, const Window* window) const;

			ref<Swapchain> createSwapchain(const Device* device, const SwapchainDescriptor& descriptor) const;

			void destroyBuffer(const Device* device, const Buffer* buffer) const;

			void destroyCommandEncoder(const Device* device, const CommandPool* commandPool, const CommandEncoder* commandEncoder) const;

			void destroyCommandPool(const Device* device, const CommandPool* commandPool) const;

			void destroyComputePipeline(const Device* device, const ComputePipeline* computePipeline) const;

			void destroyDescriptorSetLayout(const Device* device, const DescriptorSetLayout* descriptorSetLayout) const;

			void destroyDevice(const Device* device) const;

			void destroyFence(const Device* device, const Fence* fence) const;

			void destroyFramebuffer(const Device* device, const Framebuffer* framebuffer) const;

			void destroyGraphicsPipeline(const Device* device, const GraphicsPipeline* graphicsPipeline) const;

			void destroyInstance(const Instance* instance) const;

			void destroyPipelineLayout(const Device* device, const PipelineLayout* pipelineLayout) const;

			void destroyQueue(const Queue* queue) const;

			void destroySemaphore(const Device* device, const Semaphore* semaphore) const;

			void destroyShaderModule(const Device* device, const ShaderModule* shaderModule) const;

			void destroySurface(const Instance* instance, const Surface* surface) const;

			void destroySwapchain(const Device* device, const Swapchain* swapchain) const;

			ref<Queue> getQueue(const Device* device, QueueFamilyCapability queueFamilyCapability) const;

			ImageFormat getSurfaceFormat(const Surface* surface) const;

			void* mapBuffer(const Device* device, const Buffer* buffer, uint64 offset, uint64 size) const;

			PresentStatus present(const Queue* queue, view<ref<Swapchain>> swapchains, view<uint32> presentableImageIndices, ref<Semaphore> waitSemaphore) const;

			void resetFence(const Device* device, const Fence* fence) const;

			ref<PhysicalDevice> selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const;

			void submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers) const;
			void submit(const Queue* queue, view<ref<CommandBuffer>> commandBuffers, ref<Semaphore> waitSemaphore, ref<Semaphore> signalSemaphore, ref<Fence> fence) const;

			void unmapBuffer(const Device* device, const Buffer* buffer) const;

			void updateDescriptorSets(const Device* device, view<WriteDescriptorSet> writeDescriptorSets) const;

			void waitForFence(const Device* device, const Fence* fence) const;

			void waitIdle(const Device* device) const;
			void waitIdle(const Queue* queue) const;

		// TODO: Do not expose to the client side.
		public:
			AcquireNextImageImplementation acquireNextImageImplementation;
			AllocateDescriptorSetsImplementation allocateDescriptorSetsImplementation;
			CreateBufferImplementation createBufferImplementation;
			CreateCommandEncoderImplementation createCommandEncoderImplementation;
			CreateCommandPoolImplementation createCommandPoolImplementation;
			CreateComputePipelineImplementation createComputePipelineImplementation;
			CreateDescriptorSetLayoutImplementation createDescriptorSetLayoutImplementation;
			CreateDeviceImplementation createDeviceImplementation;
			CreateFenceImplementation createFenceImplementation;
			CreateFramebufferImplementation createFramebufferImplementation;
			CreateImageImplementation createImageImplementation;
			CreateInstanceImplementation createInstanceImplementation;
			CreateMemoryAllocatorImplementation createMemoryAllocatorImplementation;
			CreatePipelineLayoutImplementation createPipelineLayoutImplementation;
			CreateRenderPassImplementation createRenderPassImplementation;
			CreateGraphicsPipelineImplementation createGraphicsPipelineImplementation;
			CreateSamplerImplementation createSamplerImplementation;
			CreateSemaphoreImplementation createSemaphoreImplementation;
			CreateShaderModuleFromAssetImplementation createShaderModuleFromAssetImplementation;
			CreateShaderModuleFromCodeImplementation createShaderModuleFromCodeImplementation;
			CreateSurfaceImplementation createSurfaceImplementation;
			CreateSwapchainImplementation createSwapchainImplementation;
			DestroyBufferImplementation destroyBufferImplementation;
			DestroyCommandEncoderImplementation destroyCommandEncoderImplementation;
			DestroyCommandPoolImplementation destroyCommandPoolImplementation;
			DestroyComputePipelineImplementation destroyComputePipelineImplementation;
			DestroyDescriptorSetLayoutImplementation destroyDescriptorSetLayoutImplementation;
			DestroyDeviceImplementation destroyDeviceImplementation;
			DestroyFenceImplementation destroyFenceImplementation;
			DestroyFramebufferImplementation destroyFramebufferImplementation;
			DestroyGraphicsPipelineImplementation destroyGraphicsPipelineImplementation;
			DestroyInstanceImplementation destroyInstanceImplementation;
			DestroyPipelineLayoutImplementation destroyPipelineLayoutImplementation;
			DestroyQueueImplementation destroyQueueImplementation;
			DestroySemaphoreImplementation destroySemaphoreImplementation;
			DestroyShaderModuleImplementation destroyShaderModuleImplementation;
			DestroySurfaceImplementation destroySurfaceImplementation;
			DestroySwapchainImplementation destroySwapchainImplementation;
			GetQueueImplementation getQueueImplementation;
			GetSurfaceFormatImplementation getSurfaceFormatImplementation;
			MapBufferImplementation mapBufferImplementation;
			PresentImplementation presentImplementation;
			ResetFenceImplementation resetFenceImplementation;
			SelectPhysicalDeviceImplementation selectPhysicalDeviceImplementation;
			SubmitImplementation submitImplementation;
			Submit2Implementation submit2Implementation;
			UnmapBufferImplementation unmapBufferImplementation;
			UpdateDescriptorSetsImplementation updateDescriptorSetsImplementation;
			WaitForFenceImplementation waitForFenceImplementation;
			WaitIdleDeviceImplementation waitIdleDeviceImplementation;
			WaitIdleQueueImplementation waitIdleQueueImplementation;

		private:
			GraphicsAPI api;
	};
}