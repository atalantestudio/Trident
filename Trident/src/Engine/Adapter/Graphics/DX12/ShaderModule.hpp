// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ShaderModule.hpp"

namespace Trident {
	class DX12::ShaderModule final : public Trident::ShaderModule {
		public:
			explicit ShaderModule(const Trident::Device* device);

		public:
			ID3DBlob* blob;
	};

	namespace DX12 {
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> shaderModuleAsset);
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code);

		void destroyShaderModule(const Trident::Device* device, const Trident::ShaderModule* shaderModule);
	}
}