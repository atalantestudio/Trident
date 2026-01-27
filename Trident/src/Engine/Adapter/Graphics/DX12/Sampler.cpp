// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Filter.hpp"
#include "Engine/Adapter/Graphics/DX12/Sampler.hpp"

namespace Trident {
	DX12::Sampler::Sampler(const SamplerDescriptor& descriptor) :
		Trident::Sampler(descriptor)
	{
		samplerDescriptor.Filter = getFilter(descriptor.minFilter, descriptor.magFilter);
		samplerDescriptor.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDescriptor.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDescriptor.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDescriptor.MipLODBias = 0.0f;
		samplerDescriptor.MaxAnisotropy = 0;
		samplerDescriptor.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE;
		samplerDescriptor.BorderColor[0] = 0.0f;
		samplerDescriptor.BorderColor[1] = 0.0f;
		samplerDescriptor.BorderColor[2] = 0.0f;
		samplerDescriptor.BorderColor[3] = 1.0f;
		samplerDescriptor.MinLOD = 0.0f;
		samplerDescriptor.MaxLOD = max<FLOAT>();
	}
}