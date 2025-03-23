#pragma once

#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>

#include "Engine/Renderer/RendererAPI.hpp"

#include "Backend/Vulkan/VulkanCommandPool.hpp"
#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanInstance.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	class VulkanRendererAPI : public RendererAPI {
		public:
			VulkanRendererAPI(GLFWwindow* window);

			ref<Device> getDevice() const override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const override;

			ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const override;

			ref<Fence> createFence() const override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

			ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const override;

			ref<Semaphore> createSemaphore() const override;

			ref<ShaderModule> createShaderModule(const ShaderStage stage, const chain<uint8>& code) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const override;

			ref<Texture> createTexture(const TextureDescriptor& descriptor) const override;

		private:
			const VulkanInstance instance;
			const VulkanPhysicalDevice physicalDevice;
			const VulkanSurface surface;
			const ref<VulkanDevice> device;
			const VulkanCommandPool commandPool;
	};
}