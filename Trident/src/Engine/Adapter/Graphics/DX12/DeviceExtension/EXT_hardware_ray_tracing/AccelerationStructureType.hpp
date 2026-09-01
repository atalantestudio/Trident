// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::DX12 {
	inline D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE translateAccelerationStructureType(AccelerationStructureType type) {
		static constexpr D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE ACCELERATION_STRUCTURE_TYPES[] = {
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL,
    		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL,
		};

		ADAPTER_ASSERT(type < AccelerationStructureType::count, "Invalid acceleration structure type.");

		return ACCELERATION_STRUCTURE_TYPES[static_cast<uint32>(type)];
	}
}