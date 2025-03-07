#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class VulkanShaderModule : public ShaderModule, public ObjectWrapper<VkPipelineShaderStageCreateInfo> {
		public:
			VulkanShaderModule(const VkDevice device, const VkShaderStageFlagBits stage, const std::string& code);

			~VulkanShaderModule() override;

		private:
			const VkDevice device;
	};
}