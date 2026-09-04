// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Graphics/ShaderModule.hpp"

namespace Trident {
	class VK::ShaderModule final : public Trident::ShaderModule {
		public:
			explicit ShaderModule(const Trident::Device* device);

		public:
			VkPipelineShaderStageCreateInfo createInfo;
	};

	namespace VK {
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> shaderModuleAsset);
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, const sequence<uint8>& code);

		void destroyShaderModule(const Trident::Device* device, const Trident::ShaderModule* shaderModule);
	}
}