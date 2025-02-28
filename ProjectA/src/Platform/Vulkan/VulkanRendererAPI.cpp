#include "pch.hpp"

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
		// 9. Create graphics pipeline.
	}

	ref<ShaderModule> VulkanRendererAPI::createShaderModule(const ShaderStage stage, const std::string& code) const {
		static constexpr VkShaderStageFlagBits shaderStages[2] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		return createRef<VulkanShaderModule>(device.object, shaderStages[static_cast<uint32>(stage)], code);
	}

	ref<RenderPipeline> VulkanRendererAPI::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<VulkanRenderPipeline>(device.object, renderPass.object, descriptor);
	}
}