// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"

namespace Trident::DX12 {
	inline D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS translateAccelerationStructureBuildFlags(AccelerationStructureBuildFlags flags) {
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS dxFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

		if (flags & AccelerationStructureBuildFlags::ALLOW_UPDATE) {
			dxFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
		}

		if (flags & AccelerationStructureBuildFlags::ALLOW_COMPACTION) {
			dxFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
		}

		if (flags & AccelerationStructureBuildFlags::PREFER_FAST_TRACE) {
			dxFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		}

		if (flags & AccelerationStructureBuildFlags::PREFER_FAST_BUILD) {
			dxFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD;
		}

		if (flags & AccelerationStructureBuildFlags::MINIMIZE_MEMORY) {
			dxFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY;
		}

		return dxFlags;
	}
}