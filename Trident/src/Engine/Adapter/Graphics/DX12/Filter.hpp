// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Filter.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::DX12 {
	inline D3D12_FILTER getFilter(Filter minFilter, Filter magFilter) {
		if (minFilter == Filter::NEAREST && magFilter == Filter::NEAREST) {
			return D3D12_FILTER_MIN_MAG_MIP_POINT;
		}

		if (minFilter == Filter::NEAREST && magFilter == Filter::LINEAR) {
			// TODO(graphics/dx12/filter): Return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR instead?
			return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}

		if (minFilter == Filter::LINEAR && magFilter == Filter::NEAREST) {
			// TODO(graphics/dx12/filter): Return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT instead?
			return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		}

		if (minFilter == Filter::LINEAR && magFilter == Filter::LINEAR) {
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}

		ADAPTER_ASSERT(false, "Invalid filter.");

		return D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
}