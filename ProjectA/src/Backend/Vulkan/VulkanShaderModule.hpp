#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/ShaderModuleDescriptor.hpp"

namespace ProjectA {
	class VulkanShaderModule : public ShaderModule, public ObjectWrapper<VkPipelineShaderStageCreateInfo> {
		public:
			VulkanShaderModule(VkDevice device, const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~VulkanShaderModule() override;

		private:
			const VkDevice device;
	};
}