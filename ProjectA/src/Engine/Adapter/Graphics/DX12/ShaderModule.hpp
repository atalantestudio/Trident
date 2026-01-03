// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderModule.hpp"

namespace ProjectA {
	class DX12::ShaderModule : public ProjectA::ShaderModule {
		public:
			explicit ShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor);

		public:
			ComPtr<ID3DBlob> blob;
	};
}