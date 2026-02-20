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

	typedef void (*BindAdapterImplementation)(GraphicsAdapter* adapter);
	typedef ref<Buffer> (*CreateBufferImplementation)(const Device* device, const BufferDescriptor& descriptor, uint32 stride);
	typedef ref<ComputePipeline> (*CreateComputePipelineImplementation)(const ComputePipelineDescriptor& descriptor, const Device* device);
	typedef ref<DescriptorSetLayout> (*CreateDescriptorSetLayoutImplementation)(const sequence<DescriptorSetLayoutBinding>& bindings, const Device* device);
	typedef ref<Device> (*CreateDeviceImplementation)(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice);
	typedef ref<Fence> (*CreateFenceImplementation)(const FenceDescriptor& descriptor);
	typedef ref<Framebuffer> (*CreateFramebufferImplementation)(const FramebufferDescriptor& descriptor);
	typedef ref<Image> (*CreateImageImplementation)(const ImageDescriptor& descriptor);
	typedef ref<Instance> (*CreateInstanceImplementation)(const GraphicsAdapter* adapter, const InstanceDescriptor& instanceDescriptor);
	typedef ref<MemoryAllocator> (*CreateMemoryAllocatorImplementation)(const Device* device);
	typedef ref<PipelineLayout> (*CreatePipelineLayoutImplementation)(const PipelineLayoutDescriptor& descriptor, const Device* device);
	typedef ref<RenderPass> (*CreateRenderPassImplementation)(const RenderPassDescriptor& descriptor);
	typedef ref<RenderPipeline> (*CreateRenderPipelineImplementation)(const RenderPipelineDescriptor& descriptor);
	typedef ref<Sampler> (*CreateSamplerImplementation)(const SamplerDescriptor& descriptor);
	typedef ref<Semaphore> (*CreateSemaphoreImplementation)();
	typedef ref<ShaderModule> (*CreateShaderModuleFromAssetImplementation)(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset, const Device* device);

	// TODO: Deprecate (replace with CreateShaderModuleFromAssetImplementation).
	typedef ref<ShaderModule> (*CreateShaderModuleFromCodeImplementation)(const ShaderModuleDescriptor& descriptor, view<char8> code, const Device* device);

	typedef ref<Surface> (*CreateSurfaceImplementation)(const Instance* instance, const Window* window);
	typedef ref<Swapchain> (*CreateSwapchainImplementation)(const SwapchainDescriptor& descriptor, const Device* device);
	typedef void (*DestroyBufferImplementation)(const Buffer* buffer, const Device* device);
	typedef void* (*MapBufferImplementation)(const Device* device, const Buffer* buffer, uint64 offset, uint64 size);
	typedef ref<PhysicalDevice> (*SelectPhysicalDeviceImplementation)(const GraphicsAdapter* adapter, const Instance* instance, bool preferIntegratedGpu);
	typedef void (*UnmapBufferImplementation)(const Device* device, const Buffer* buffer);

	class GraphicsAdapter {
		public:
			static ConsoleLogger logger;

		public:
			explicit GraphicsAdapter(const ApplicationProperties& properties);

			GraphicsAPI getApi() const;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor, const Device* device) const;

			template<typename ElementType>
			ref<Buffer2<ElementType>> createBuffer2(const BufferDescriptor& descriptor, const Device* device) const {
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				ref<Buffer> buffer = createBufferImplementation(device, descriptor, static_cast<uint32>(sizeof(ElementType)));

				return std::static_pointer_cast<Buffer2<ElementType>>(buffer);
			}

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor, const Device* device) const;

			ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings, const Device* device) const;

			ref<Device> createDevice(const PhysicalDevice* physicalDevice) const;

			ref<Fence> createFence(const FenceDescriptor& descriptor) const;

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const;

			ref<Image> createImage(const ImageDescriptor& descriptor) const;

			ref<Instance> createInstance(const InstanceDescriptor& descriptor) const;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor, const Device* device) const;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore() const;

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset, const Device* device) const;
			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code, const Device* device) const;

			ref<Surface> createSurface(const Instance* instance, const Window* window) const;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor, const Device* device) const;

			void destroyBuffer(const Buffer* buffer, const Device* device) const;

			void* mapBuffer(const Device* device, const Buffer* buffer, uint64 offset, uint64 size) const;

			ref<PhysicalDevice> selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const;

			void unmapBuffer(const Device* device, const Buffer* buffer) const;

		// TODO: Do not expose to the client side.
		public:
			CreateBufferImplementation createBufferImplementation;
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
			MapBufferImplementation mapBufferImplementation;
			SelectPhysicalDeviceImplementation selectPhysicalDeviceImplementation;
			UnmapBufferImplementation unmapBufferImplementation;

		private:
			GraphicsAPI api;
	};
}