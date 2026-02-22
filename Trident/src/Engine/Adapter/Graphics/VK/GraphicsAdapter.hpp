// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Instance.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/Application/ApplicationProperties.hpp"

namespace Trident {
	class VK::GraphicsAdapter final : public Trident::GraphicsAdapter {
		public:
			explicit GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties);

			// ~GraphicsAdapter() final override;

			ref<Trident::Buffer> createBuffer(const BufferDescriptor& descriptor) const final override;

			ref<Trident::ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const final override;

			ref<Trident::DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const final override;

			ref<Trident::Fence> createFence(const FenceDescriptor& descriptor) const final override;

			ref<Trident::Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const final override;

			ref<Trident::Image> createImage(const ImageDescriptor& descriptor) const final override;

			ref<Trident::PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const final override;

			ref<Trident::RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const final override;

			ref<Trident::GraphicsPipeline> createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) const final override;

			ref<Trident::Sampler> createSampler(const SamplerDescriptor& descriptor) const final override;

			ref<Trident::Semaphore> createSemaphore() const final override;

			ref<Trident::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const final override;
			ref<Trident::ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const final override;

			ref<Trident::Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const final override;

			ref<Trident::Device> getDevice() const final override;

			ref<Trident::Surface> getSurface() const final override;

		private:
			void* getProcedure(VkInstance instance, view<char8> procedureName) const;

			void loadBaseProcedures() const;

			void loadInstanceProcedures() const;

		private:
			PlatformLibrary vulkan1Library;
			ref<VK::Instance> instance;
			ref<VK::Surface> surface;
			ref<VK::PhysicalDevice> physicalDevice;
			ref<VK::Device> device;
	};
}