#pragma once

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/ShaderModule.hpp"

namespace ProjectA {
	class VulkanShaderModule : public ObjectWrapper<VkShaderModule>, public ShaderModule {
		public:
			VulkanShaderModule(const VkDevice& device, const VkShaderStageFlagBits stage, const std::string& code);

			~VulkanShaderModule() override;

		private:
			const VkDevice& device;
	};
}