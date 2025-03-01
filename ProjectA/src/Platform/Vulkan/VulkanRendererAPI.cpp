#include "pch.hpp"

#include "Platform/Vulkan/VulkanBuffer.hpp"
#include "Platform/Vulkan/VulkanRendererAPI.hpp"
#include "Platform/Vulkan/VulkanRenderPipeline.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanRendererAPI::VulkanRendererAPI(GLFWwindow* window) :
		instance(),
		physicalDevice(instance.object),
		surface(instance.object, physicalDevice, window),
		device(physicalDevice, surface),
		swapchain(physicalDevice, surface, device.object),
		renderPass(surface, device.object)
	{
		// 1. Create instance.
		// 2. Select physical device.
		// 3. Create surface.
		// 4. Check if the physical device supports the surface.
		// 5. Get capabilities, extent, format and present mode of the surface with the physical device.
		// 6. Create device.
		// 7. Create swapchain.
		// 8. Create render pass.
		// 9. Create pipeline layout.
		// 10. Create graphics pipeline.
		// 11. Create buffer.
		// 12. Allocate memory for the buffer.
	}

	ref<Buffer> VulkanRendererAPI::createBuffer(const BufferDescriptor& descriptor) {
		return createRef<VulkanBuffer>(physicalDevice.object, device, descriptor);
	}

	ref<PipelineLayout> VulkanRendererAPI::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<VulkanPipelineLayout>(device.object, descriptor);
	}

	ref<RenderPipeline> VulkanRendererAPI::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<VulkanRenderPipeline>(device.object, swapchain, renderPass.object, descriptor);
	}

	ref<ShaderModule> VulkanRendererAPI::createShaderModule(const ShaderStage stage, const std::string& code) const {
		static constexpr VkShaderStageFlagBits shaderStages[] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		return createRef<VulkanShaderModule>(device.object, shaderStages[static_cast<uint32>(stage)], code);
	}
}