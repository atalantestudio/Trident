// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/SamplerFilter.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::DX12 {
	inline D3D12_FILTER getFilter(SamplerFilter minFilter, SamplerFilter magFilter) {
		if (minFilter == SamplerFilter::NEAREST && magFilter == SamplerFilter::NEAREST) {
			return D3D12_FILTER_MIN_MAG_MIP_POINT;
		}

		if (minFilter == SamplerFilter::NEAREST && magFilter == SamplerFilter::LINEAR) {
			// TODO: Return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR instead?
			return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}

		if (minFilter == SamplerFilter::LINEAR && magFilter == SamplerFilter::NEAREST) {
			// TODO: Return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT instead?
			return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		}

		if (minFilter == SamplerFilter::LINEAR && magFilter == SamplerFilter::LINEAR) {
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}

		ADAPTER_ASSERT(false, "Unknown sampler filter.");

		return D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
}