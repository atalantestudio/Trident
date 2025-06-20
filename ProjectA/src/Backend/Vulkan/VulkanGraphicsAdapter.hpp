#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanInstance.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/GraphicsAdapter.hpp"

namespace ProjectA {
	class VulkanGraphicsAdapter final : public GraphicsAdapter {
		public:
			VulkanGraphicsAdapter(const Window& window);

			~VulkanGraphicsAdapter() final override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const final override;

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const final override;

			ref<Fence> createFence() const final override;

			ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const final override;

			ref<Image> createImage(const ImageDescriptor& descriptor) const final override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const final override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const final override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const final override;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const final override;

			ref<Semaphore> createSemaphore() const final override;

			ref<ShaderModule> createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const final override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const final override;

			ref<Device> getDevice() const final override;

			ref<Surface> getSurface() const final override;

		private:
			const VulkanInstance instance;
			const ref<VulkanSurface> surface;
			const VulkanPhysicalDevice physicalDevice;
			const ref<VulkanDevice> device;
	};
}