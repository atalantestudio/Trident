// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Rendering/Sampler.hpp"

namespace ProjectA {
	class DX12::Sampler final : public ProjectA::Sampler {
		public:
			explicit Sampler(const SamplerDescriptor& descriptor, const DX12::Device& device);

		private:
			D3D12_CPU_DESCRIPTOR_HANDLE samplerHandle;
	};
}