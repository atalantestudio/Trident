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
	class Window;

	typedef AcquireNextImageStatus (*AcquireNextImageImplementation)(const Device* device, const Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex);
	typedef sequence<ref<DescriptorSet>> (*AllocateDescriptorSetsImplementation)(const Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts);
	typedef void (*BindAdapterImplementation)(GraphicsAdapter* adapter);
	typedef ref<Buffer> (*CreateBufferImplementation)(const Device* device, const BufferDescriptor& descriptor, uint32 stride);
	typedef ref<CommandPool> (*CreateCommandPoolImplementation)(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability);
	typedef ref<ComputePipeline> (*CreateComputePipelineImplementation)(const ComputePipelineDescriptor& descriptor, const Device* device);
	typedef ref<DescriptorSetLayout> (*CreateDescriptorSetLayoutImplementation)(const sequence<DescriptorSetLayoutBinding>& bindings, const Device* device);
	typedef ref<Device> (*CreateDeviceImplementation)(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice);
	typedef ref<Fence> (*CreateFenceImplementation)(const Device* device, const FenceDescriptor& descriptor);
	typedef ref<Framebuffer> (*CreateFramebufferImplementation)(const Device* device, const FramebufferDescriptor& descriptor);
	typedef ref<Image> (*CreateImageImplementation)(const ImageDescriptor& descriptor);
	typedef ref<Instance> (*CreateInstanceImplementation)(const GraphicsAdapter* adapter, const InstanceDescriptor& instanceDescriptor);
	typedef ref<MemoryAllocator> (*CreateMemoryAllocatorImplementation)(const Device* device);
	typedef ref<PipelineLayout> (*CreatePipelineLayoutImplementation)(const PipelineLayoutDescriptor& descriptor, const Device* device);
	typedef ref<RenderPass> (*CreateRenderPassImplementation)(const Device* device, const RenderPassDescriptor& descriptor);
	typedef ref<RenderPipeline> (*CreateRenderPipelineImplementation)(const RenderPipelineDescriptor& descriptor);
	typedef ref<Sampler> (*CreateSamplerImplementation)(const SamplerDescriptor& descriptor);
	typedef ref<Semaphore> (*CreateSemaphoreImplementation)();
	typedef ref<ShaderModule> (*CreateShaderModuleFromAssetImplementation)(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset, const Device* device);

	// TODO: Deprecate (replace with CreateShaderModuleFromAssetImplementation).
	typedef ref<ShaderModule> (*CreateShaderModuleFromCodeImplementation)(const ShaderModuleDescriptor& descriptor, view<char8> code, const Device* device);

	typedef ref<Surface> (*CreateSurfaceImplementation)(const Instance* instance, const Window* window);
	typedef ref<Swapchain> (*CreateSwapchainImplementation)(const Device* device, const SwapchainDescriptor& descriptor);
	typedef void (*DestroyBufferImplementation)(const Device* device, const Buffer* buffer);
	typedef void (*DestroyCommandPoolImplementation)(const Device* device, const CommandPool* commandPool);
	typedef void (*DestroyDeviceImplementation)(const Device* device);
	typedef void (*DestroyFenceImplementation)(const Device* device, const Fence* fence);
	typedef void (*DestroyFramebufferImplementation)(const Device* device, const Framebuffer* framebuffer);
	typedef void (*DestroyInstanceImplementation)(const Instance* instance);
	typedef void (*DestroyQueueImplementation)(const Queue* queue);
	typedef void (*DestroySurfaceImplementation)(const Instance* instance, const Surface* surface);
	typedef void (*DestroySwapchainImplementation)(const Device* device, const Swapchain* swapchain);
	typedef ref<Queue> (*GetQueueImplementation)(const Device* device, QueueFamilyCapability queueFamilyCapability);
	typedef ImageFormat (*GetSurfaceFormatImplementation)(const Surface* surface);
	typedef void* (*MapBufferImplementation)(const Device* device, const Buffer* buffer, uint64 offset, uint64 size);
	typedef void (*ResetFenceImplementation)(const Device* device, const Fence* fence);
	typedef ref<PhysicalDevice> (*SelectPhysicalDeviceImplementation)(const Instance* instance, bool preferIntegratedGpu);
	typedef void (*UnmapBufferImplementation)(const Device* device, const Buffer* buffer);
	typedef void (*UpdateDescriptorSetsImplementation)(const Device* device, view<WriteDescriptorSet> writeDescriptorSets);
	typedef void (*WaitForFenceImplementation)(const Device* device, const Fence* fence);
	typedef void (*WaitIdleImplementation)(const Device* device);

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

			ref<CommandPool> createCommandPool(const Device* device, uint32 queueFamilyIndex, QueueFamilyCapability queueFamilyCapability) const;

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor, const Device* device) const;

			ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings, const Device* device) const;

			ref<Device> createDevice(const PhysicalDevice* physicalDevice) const;

			ref<Fence> createFence(const Device* device, const FenceDescriptor& descriptor) const;

			ref<Framebuffer> createFramebuffer(const Device* device, const FramebufferDescriptor& descriptor) const;

			ref<Image> createImage(const ImageDescriptor& descriptor) const;

			ref<Instance> createInstance(const InstanceDescriptor& descriptor) const;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor, const Device* device) const;

			ref<RenderPass> createRenderPass(const Device* device, const RenderPassDescriptor& descriptor) const;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore() const;

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset, const Device* device) const;
			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code, const Device* device) const;

			ref<Surface> createSurface(const Instance* instance, const Window* window) const;

			ref<Swapchain> createSwapchain(const Device* device, const SwapchainDescriptor& descriptor) const;

			void destroyBuffer(const Device* device, const Buffer* buffer) const;

			void destroyCommandPool(const Device* device, const CommandPool* commandPool) const;

			void destroyDevice(const Device* device) const;

			void destroyFence(const Device* device, const Fence* fence) const;

			void destroyFramebuffer(const Device* device, const Framebuffer* framebuffer) const;

			void destroyInstance(const Instance* instance) const;

			void destroyQueue(const Queue* queue) const;

			void destroySurface(const Instance* instance, const Surface* surface) const;

			void destroySwapchain(const Device* device, const Swapchain* swapchain) const;

			ref<Queue> getQueue(const Device* device, QueueFamilyCapability queueFamilyCapability) const;

			ImageFormat getSurfaceFormat(const Surface* surface) const;

			void* mapBuffer(const Device* device, const Buffer* buffer, uint64 offset, uint64 size) const;

			void resetFence(const Device* device, const Fence* fence) const;

			ref<PhysicalDevice> selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const;

			void unmapBuffer(const Device* device, const Buffer* buffer) const;

			void updateDescriptorSets(const Device* device, view<WriteDescriptorSet> writeDescriptorSets) const;

			void waitForFence(const Device* device, const Fence* fence) const;

			void waitIdle(const Device* device) const;

		// TODO: Do not expose to the client side.
		public:
			AcquireNextImageImplementation acquireNextImageImplementation;
			AllocateDescriptorSetsImplementation allocateDescriptorSetsImplementation;
			CreateBufferImplementation createBufferImplementation;
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
			CreateRenderPipelineImplementation createRenderPipelineImplementation;
			CreateSamplerImplementation createSamplerImplementation;
			CreateSemaphoreImplementation createSemaphoreImplementation;
			CreateShaderModuleFromAssetImplementation createShaderModuleFromAssetImplementation;
			CreateShaderModuleFromCodeImplementation createShaderModuleFromCodeImplementation;
			CreateSurfaceImplementation createSurfaceImplementation;
			CreateSwapchainImplementation createSwapchainImplementation;
			DestroyBufferImplementation destroyBufferImplementation;
			DestroyCommandPoolImplementation destroyCommandPoolImplementation;
			DestroyDeviceImplementation destroyDeviceImplementation;
			DestroyFenceImplementation destroyFenceImplementation;
			DestroyFramebufferImplementation destroyFramebufferImplementation;
			DestroyInstanceImplementation destroyInstanceImplementation;
			DestroyQueueImplementation destroyQueueImplementation;
			DestroySurfaceImplementation destroySurfaceImplementation;
			DestroySwapchainImplementation destroySwapchainImplementation;
			GetQueueImplementation getQueueImplementation;
			GetSurfaceFormatImplementation getSurfaceFormatImplementation;
			MapBufferImplementation mapBufferImplementation;
			ResetFenceImplementation resetFenceImplementation;
			SelectPhysicalDeviceImplementation selectPhysicalDeviceImplementation;
			UnmapBufferImplementation unmapBufferImplementation;
			UpdateDescriptorSetsImplementation updateDescriptorSetsImplementation;
			WaitForFenceImplementation waitForFenceImplementation;
			WaitIdleImplementation waitIdleImplementation;

		private:
			GraphicsAPI api;
	};
}