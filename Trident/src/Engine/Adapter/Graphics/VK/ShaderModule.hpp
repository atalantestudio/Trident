// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"

namespace Trident {
	class VK::ShaderModule final : public Trident::ShaderModule {
		public:
			explicit ShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code, const VK::Device& device);

			~ShaderModule() final override;

		public:
			VkPipelineShaderStageCreateInfo createInfo;

		private:
			const VK::Device& device;
	};
}