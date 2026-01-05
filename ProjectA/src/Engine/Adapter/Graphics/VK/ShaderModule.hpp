#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace ProjectA {
	class VulkanShaderModule final : public ShaderModule, public ObjectWrapper<VkPipelineShaderStageCreateInfo> {
		public:
			explicit VulkanShaderModule(VkDevice device, const ref<ShaderModuleAsset>& asset, const ShaderModuleDescriptor& descriptor);

			// TODO: Deprecate.
			explicit VulkanShaderModule(view<char8> code, const ShaderModuleDescriptor& descriptor, VkDevice device);

			~VulkanShaderModule() final override;

		private:
			const VkDevice device;
	};
}