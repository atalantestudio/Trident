#pragma once

#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>

#include "Engine/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanRenderPass.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	class VulkanRendererAPI : public RendererAPI {
		public:
			VulkanRendererAPI(GLFWwindow* window);

			// TODO: Implement
			void setViewport(int32 x, int32, uint32 width, uint32 height) const override {}

			// TODO: Implement
			void setClearColor(const vec4<float32>& color) const override {}

			// TODO: Implement
			void clear() const override {}

			// TODO: Implement
			void drawIndexed(const ref<RenderPipeline>& renderPipeline, uint32 indexCount) const override {}

			ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code) const override;

			ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const override;

		private:
			const VulkanInstance instance;
			const VulkanPhysicalDevice physicalDevice;
			const VulkanSurface surface;
			const VulkanDevice device;
			const VulkanSwapchain swapchain;
			const VulkanRenderPass renderPass;
	};
}