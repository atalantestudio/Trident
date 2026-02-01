// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace Trident::DX12 {
	D3D12_DESCRIPTOR_RANGE_TYPE getDescriptorRangeType(DescriptorType descriptorType) {
		switch (descriptorType) {
			case DescriptorType::COMBINED_IMAGE_SAMPLER: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			case DescriptorType::STORAGE_IMAGE: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case DescriptorType::INPUT_ATTACHMENT: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case DescriptorType::UNIFORM_BUFFER: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case DescriptorType::READ_ONLY_STORAGE_BUFFER: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case DescriptorType::READ_WRITE_STORAGE_BUFFER: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			default:
				ADAPTER_ASSERT(false, "Undefined descriptor range type.");

				return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		}
	}
}