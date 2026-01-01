#pragma once

#include "Backend/VK/Device.hpp"
#include "Backend/VK/Instance.hpp"
#include "Backend/VK/PipelineLayout.hpp"
#include "Backend/VK/RenderPass.hpp"
#include "Backend/VK/Surface.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class VulkanGraphicsAdapter final : public GraphicsAdapter {
		public:
			explicit VulkanGraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window);

			~VulkanGraphicsAdapter() final override;

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

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const final override;

			ref<Device> getDevice() const final override;

			ref<Surface> getSurface() const final override;

		private:
			void* getProcedure(VkInstance instance, std::string_view procedureName) const;

			void loadBaseProcedures() const;

			void loadInstanceProcedures() const;

		private:
			PlatformLibrary vulkan1Library;
			ref<VulkanInstance> instance;
			ref<VulkanSurface> surface;
			ref<VulkanPhysicalDevice> physicalDevice;
			ref<VulkanDevice> device;
	};
}