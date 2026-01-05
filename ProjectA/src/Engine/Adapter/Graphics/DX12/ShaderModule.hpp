// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderModule.hpp"

namespace ProjectA {
	class DX12::ShaderModule final : public ProjectA::ShaderModule {
		public:
			explicit ShaderModule(view<char8> code, const ShaderModuleDescriptor& descriptor);

		public:
			ComPtr<ID3DBlob> blob;
	};
}