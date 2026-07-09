// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Base.hpp"

#include "Engine/Adapter/Graphics/DX12/Filter.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"

namespace Trident {
	DX12::Sampler::Sampler(const Trident::Device* device) :
		Trident::Sampler(device)
	{}

	ref<Trident::Sampler> DX12::createSampler(const Trident::Device* device, const SamplerDescriptor& descriptor) {
		ref<DX12::Sampler> sampler = createRef<DX12::Sampler>(device);
		sampler->descriptor.Filter = getFilter(descriptor.minFilter, descriptor.magFilter);
		sampler->descriptor.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler->descriptor.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler->descriptor.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler->descriptor.MipLODBias = 0.0f;
		sampler->descriptor.MaxAnisotropy = 1;

		#if COMPILER == COMPILER_GCC
			// TODO(ci/gcc): D3D12_COMPARISON_FUNC_NONE does not seem to be in the header pulled by GCC.
			sampler->descriptor.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(0);
		#else
			sampler->descriptor.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE;
		#endif

		sampler->descriptor.BorderColor[0] = 0.0f;
		sampler->descriptor.BorderColor[1] = 0.0f;
		sampler->descriptor.BorderColor[2] = 0.0f;
		sampler->descriptor.BorderColor[3] = 1.0f;
		sampler->descriptor.MinLOD = 0.0f;
		sampler->descriptor.MaxLOD = 1000.0f;

		return sampler;
	}

	void DX12::destroySampler(const Trident::Device* device, const Trident::Sampler* sampler) {}
}