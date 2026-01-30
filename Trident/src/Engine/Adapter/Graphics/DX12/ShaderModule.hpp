// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderModule.hpp"

namespace Trident {
	class DX12::ShaderModule final : public Trident::ShaderModule {
		public:
			explicit ShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code);

			~ShaderModule() final override;

		public:
			ID3DBlob* blob;
	};
}