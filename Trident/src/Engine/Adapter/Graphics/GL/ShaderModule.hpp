// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	class GL::ShaderModule final : public Trident::ShaderModule {
		public:
			explicit ShaderModule(const Trident::Device* device);

		public:
			GLuint shader;
	};

	namespace GL {
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> shaderModuleAsset);
		ref<Trident::ShaderModule> createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code);

		void destroyShaderModule(const Trident::Device* device, const Trident::ShaderModule* shaderModule);
	}
}