#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"

namespace ProjectA {
	class VulkanShaderModule final : public ShaderModule, public ObjectWrapper<VkPipelineShaderStageCreateInfo> {
		public:
			VulkanShaderModule(VkDevice device, const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~VulkanShaderModule() final override;

		private:
			const VkDevice device;
	};
}