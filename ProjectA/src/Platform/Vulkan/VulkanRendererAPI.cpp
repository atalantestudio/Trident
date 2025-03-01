#include "pch.hpp"

#include "Platform/Vulkan/VulkanBuffer.hpp"
#include "Platform/Vulkan/VulkanCommandBuffer.hpp"
#include "Platform/Vulkan/VulkanRendererAPI.hpp"
#include "Platform/Vulkan/VulkanRenderPipeline.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanRendererAPI::VulkanRendererAPI(GLFWwindow* window) :
		instance(),
		physicalDevice(instance.object),
		surface(instance.object, physicalDevice, window),
		device(physicalDevice, surface),
		commandPool(physicalDevice, device.object)
	{
		// 1. Create instance.
		// 2. Select physical device.
		// 3. Create surface.
		// 4. Check if the physical device supports the surface.
		// 5. Get capabilities, extent, format and present mode of the surface with the physical device.
		// 6. Create device.
		// 7.Create command pool.

		// 1. Create swapchain.
		// 2. Create render pass.
		// 3. Create pipeline layout.
		// 4. Create graphics pipeline.
		// 5. Create buffer.
		// 6. Allocate memory for the buffer.
	}

	ref<Buffer> VulkanRendererAPI::createBuffer(const BufferDescriptor& descriptor) {
		return createRef<VulkanBuffer>(physicalDevice.object, device, descriptor);
	}

	ref<CommandBuffer> VulkanRendererAPI::createCommandBuffer() const {
		return createRef<VulkanCommandBuffer>(device.object, commandPool.object);
	}

	ref<PipelineLayout> VulkanRendererAPI::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<VulkanPipelineLayout>(device.object, descriptor);
	}

	ref<RenderPass> VulkanRendererAPI::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<VulkanRenderPass>(surface, device.object, descriptor);
	}

	ref<RenderPipeline> VulkanRendererAPI::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<VulkanRenderPipeline>(device.object, descriptor);
	}

	ref<ShaderModule> VulkanRendererAPI::createShaderModule(const ShaderStage stage, const std::string& code) const {
		static constexpr VkShaderStageFlagBits shaderStages[] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		return createRef<VulkanShaderModule>(device.object, shaderStages[static_cast<uint32>(stage)], code);
	}

	ref<Swapchain> VulkanRendererAPI::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<VulkanSwapchain>(physicalDevice, surface, device.object, descriptor);
	}
}