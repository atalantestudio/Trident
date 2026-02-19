// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	class Window;

	typedef void (*BindAdapterImplementation)(GraphicsAdapter* adapter);

	typedef ref<Buffer> (*CreateBufferImplementation)(const BufferDescriptor& descriptor);
	typedef ref<ComputePipeline> (*CreateComputePipelineImplementation)(const ComputePipelineDescriptor& descriptor);

	// TODO: Use a view of DescriptorSetLayoutBinding instead.
	typedef ref<DescriptorSetLayout> (*CreateDescriptorSetLayoutImplementation)(const sequence<DescriptorSetLayoutBinding>& bindings);

	typedef ref<Device> (*CreateDeviceImplementation)(const Trident::GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice);

	typedef ref<Fence> (*CreateFenceImplementation)(const FenceDescriptor& descriptor);
	typedef ref<Framebuffer> (*CreateFramebufferImplementation)(const FramebufferDescriptor& descriptor);
	typedef ref<Image> (*CreateImageImplementation)(const ImageDescriptor& descriptor);
	typedef ref<Instance> (*CreateInstanceImplementation)(const GraphicsAdapter* adapter, const InstanceDescriptor& instanceDescriptor);
	typedef ref<PipelineLayout> (*CreatePipelineLayoutImplementation)(const PipelineLayoutDescriptor& descriptor);
	typedef ref<RenderPass> (*CreateRenderPassImplementation)(const RenderPassDescriptor& descriptor);
	typedef ref<RenderPipeline> (*CreateRenderPipelineImplementation)(const RenderPipelineDescriptor& descriptor);
	typedef ref<Sampler> (*CreateSamplerImplementation)(const SamplerDescriptor& descriptor);
	typedef ref<Semaphore> (*CreateSemaphoreImplementation)();
	typedef ref<ShaderModule> (*CreateShaderModuleFromAssetImplementation)(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset);

	// TODO: Deprecate (replace with CreateShaderModuleFromAssetImplementation).
	typedef ref<ShaderModule> (*CreateShaderModuleFromPathImplementation)(const ShaderModuleDescriptor& descriptor, view<char8> code);

	typedef ref<Surface> (*CreateSurfaceImplementation)(const Instance* instance, const Window* window);
	typedef ref<Swapchain> (*CreateSwapchainImplementation)(const SwapchainDescriptor& descriptor, const Device* device);
	typedef ref<PhysicalDevice> (*SelectPhysicalDeviceImplementation)(const GraphicsAdapter* adapter, const Instance* instance, bool preferIntegratedGpu);

	class GraphicsAdapter {
		public:
			static ConsoleLogger logger;

		public:
			explicit GraphicsAdapter(const ApplicationProperties& properties);

			GraphicsAPI getApi() const;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const;

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const;

			ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const;

			ref<Device> createDevice(const PhysicalDevice* physicalDevice) const;

			ref<Fence> createFence(const FenceDescriptor& descriptor) const;

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const;

			ref<Image> createImage(const ImageDescriptor& descriptor) const;

			ref<Instance> createInstance(const InstanceDescriptor& descriptor) const;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore() const;

			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const;

			// TODO: Deprecate (replace with CreateShaderModuleFromAssetProcedure).
			ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const;

			ref<Surface> createSurface(const Instance* instance, const Window* window) const;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor, const Device* device) const;

			ref<PhysicalDevice> selectPhysicalDevice(const Instance* instance, bool preferIntegratedGpu) const;

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
			CreatePipelineLayoutImplementation createPipelineLayoutImplementation;
			CreateRenderPassImplementation createRenderPassImplementation;
			CreateRenderPipelineImplementation createRenderPipelineImplementation;
			CreateSamplerImplementation createSamplerImplementation;
			CreateSemaphoreImplementation createSemaphoreImplementation;
			CreateShaderModuleFromAssetImplementation createShaderModuleFromAssetImplementation;
			CreateShaderModuleFromPathImplementation createShaderModuleFromPathImplementation;
			CreateSurfaceImplementation createSurfaceImplementation;
			CreateSwapchainImplementation createSwapchainImplementation;
			SelectPhysicalDeviceImplementation selectPhysicalDeviceImplementation;

		private:
			GraphicsAPI api;
	};
}