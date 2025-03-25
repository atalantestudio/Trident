#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class VulkanShaderModule : public ShaderModule, public ObjectWrapper<VkPipelineShaderStageCreateInfo> {
		public:
			VulkanShaderModule(VkDevice device, VkShaderStageFlagBits stage, const chain<uint8>& code);

			~VulkanShaderModule() override;

		private:
			const VkDevice device;
	};
}