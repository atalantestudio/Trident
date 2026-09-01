// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"

namespace Trident::DX12 {
	inline D3D12_RAYTRACING_GEOMETRY_FLAGS translateGeometryFlags(GeometryFlags flags) {
		return static_cast<D3D12_RAYTRACING_GEOMETRY_FLAGS>(flags);
	}
}