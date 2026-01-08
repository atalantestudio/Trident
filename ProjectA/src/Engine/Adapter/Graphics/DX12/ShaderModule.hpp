// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderModule.hpp"

namespace ProjectA {
	class DX12::ShaderModule final : public ProjectA::ShaderModule {
		public:
			explicit ShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code);

		public:
			ComPtr<ID3DBlob> blob;
	};
}