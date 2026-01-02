// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Backend/DX12/Device.hpp"
#include "Backend/DX12/Instance.hpp"
#include "Backend/DX12/Surface.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	namespace DX12 {
		class GraphicsAdapter;
	}

	class DX12::GraphicsAdapter final : public ProjectA::GraphicsAdapter {
		public:
			explicit GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window);

			~GraphicsAdapter() final override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const final override;

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const final override;

			ref<DescriptorSetLayout> createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const final override;

			ref<Fence> createFence() const final override;

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const final override;

			ref<Image> createImage(const ImageDescriptor& descriptor) const final override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const final override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const final override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const final override;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const final override;

			ref<Semaphore> createSemaphore() const final override;

			ref<ShaderModule> createShaderModule(const ref<ShaderModuleAsset>& asset, const ShaderModuleDescriptor& descriptor) const final override;
			ref<ShaderModule> createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const final override;

			ref<ProjectA::Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const final override;

			ref<ProjectA::Device> getDevice() const final override;

			ref<ProjectA::Surface> getSurface() const final override;

		private:
			ref<Instance> instance;
			ref<Surface> surface;
			ref<Device> device;
	};
}