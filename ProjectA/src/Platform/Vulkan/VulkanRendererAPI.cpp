#include "pch.hpp"

#include "Platform/Vulkan/VulkanRendererAPI.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanRendererAPI::VulkanRendererAPI() : instance(), device(instance.object) {}

	ref<ShaderModule> VulkanRendererAPI::createShaderModule(const ShaderStage stage, const std::string& code) const {
		static constexpr VkShaderStageFlagBits shaderStages[2] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		return createRef<VulkanShaderModule>(device.object, shaderStages[static_cast<uint32>(stage)], code);
	}
}