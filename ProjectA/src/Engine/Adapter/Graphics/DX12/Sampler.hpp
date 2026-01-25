// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Sampler.hpp"

namespace ProjectA {
	class DX12::Sampler final : public ProjectA::Sampler {
		public:
			explicit Sampler(const SamplerDescriptor& descriptor);

		public:
			D3D12_SAMPLER_DESC samplerDescriptor;
	};
}