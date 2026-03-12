// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Sampler.hpp"

namespace Trident {
	class DX12::Sampler final : public Trident::Sampler {
		public:
			explicit Sampler(const Trident::Device* device);

		public:
			D3D12_SAMPLER_DESC descriptor;
	};

	namespace DX12 {
		ref<Trident::Sampler> createSampler(const Trident::Device* device, const SamplerDescriptor& descriptor);

		void destroySampler(const Trident::Device* device, const Trident::Sampler* sampler);
	}
}