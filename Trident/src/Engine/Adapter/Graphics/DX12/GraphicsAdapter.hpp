// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/Application/ApplicationProperties.hpp"

namespace Trident {
	class DX12::GraphicsAdapter final : public Trident::GraphicsAdapter {
		public:
			explicit GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties);

		private:
			ref<Trident::Buffer> createBuffer(const BufferDescriptor& descriptor) const;

			ref<Trident::ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const;

			ref<Trident::DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const;

			ref<Trident::Fence> createFence(const FenceDescriptor& descriptor) const;

			ref<Trident::Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const;

			ref<Trident::Image> createImage(const ImageDescriptor& descriptor) const;

			ref<Trident::PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const;

			ref<Trident::RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const;

			ref<Trident::Sampler> createSampler(const SamplerDescriptor& descriptor) const;

			ref<Semaphore> createSemaphore() const;

			ref<Trident::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const;
			ref<Trident::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const;

			ref<Trident::Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const;

			ref<Trident::Device> getDevice() const;

			ref<Trident::Surface> getSurface() const;

		private:
			ref<DX12::Instance> instance;
			ref<DX12::Surface> surface;
			ref<DX12::PhysicalDevice> physicalDevice;
			ref<DX12::Device> device;
			ref<DX12::MemoryAllocator> memoryAllocator;
	};

	namespace DX12 {
		void bindAdapter(Trident::GraphicsAdapter* adapter);

		ref<Trident::Device> createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		ref<Trident::Instance> createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor);

		ref<Trident::Surface> createSurface(const Trident::Instance* instance, const Window* window);

		ref<Trident::PhysicalDevice> selectPhysicalDevice(const Trident::GraphicsAdapter* adapter, const Trident::Instance* instance, bool preferIntegratedGpu);
	}
}