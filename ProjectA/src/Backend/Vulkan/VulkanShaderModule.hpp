#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace ProjectA {
	class VulkanShaderModule final : public ShaderModule, public ObjectWrapper<VkPipelineShaderStageCreateInfo> {
		public:
			explicit VulkanShaderModule(VkDevice device, const ref<ShaderModuleAsset>& asset, const ShaderModuleDescriptor& descriptor);

			// TODO: Deprecate.
			explicit VulkanShaderModule(VkDevice device, const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor);

			~VulkanShaderModule() final override;

		private:
			const VkDevice device;
	};
}