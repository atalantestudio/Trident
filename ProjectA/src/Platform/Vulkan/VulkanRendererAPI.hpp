#pragma once

#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>

#include "Engine/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/VulkanCommandPool.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanPipelineLayout.hpp"
#include "Platform/Vulkan/VulkanRenderPass.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	class VulkanRendererAPI : public RendererAPI {
		public:
			VulkanRendererAPI(GLFWwindow* window);

			ref<Device> getDevice() const override;

			ref<Buffer> createBuffer(const BufferDescriptor& descriptor) override;

			ref<CommandBuffer> createCommandBuffer() const override;

			ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const override;

			ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

			ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code) const override;

			ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const override;

		private:
			const VulkanInstance instance;
			const VulkanPhysicalDevice physicalDevice;
			const VulkanSurface surface;
			ref<VulkanDevice> device;
			const VulkanCommandPool commandPool;
	};
}