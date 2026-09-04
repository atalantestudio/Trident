// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ShaderModule.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	class GL::ShaderModule final : public Trident::ShaderModule {
		public:
			explicit ShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor);

		public:
			GLuint shader;
	};

	namespace GL {
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> shaderModuleAsset);
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, const sequence<uint8>& code);

		void destroyShaderModule(const Trident::Device* device, const Trident::ShaderModule* shaderModule);
	}
}