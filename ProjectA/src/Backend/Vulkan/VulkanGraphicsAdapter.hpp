#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanInstance.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/GraphicsAdapter.hpp"

namespace ProjectA {
	class VulkanGraphicsAdapter : public GraphicsAdapter {
		public:
			VulkanGraphicsAdapter(const Window& window);

			ref<Device> getDevice() const override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const override;

			ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const override;

			ref<Fence> createFence() const override;

			ref<Image> createImage(const ImageDescriptor& descriptor) const override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const override;

			ref<Semaphore> createSemaphore() const override;

			ref<ShaderModule> createShaderModule(const chain<uint8>& code, const ShaderModuleDescriptor& descriptor) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const override;

		private:
			const VulkanInstance instance;
			const VulkanPhysicalDevice physicalDevice;
			const VulkanSurface surface;
			const ref<VulkanDevice> device;
	};
}