// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildTriangleGeometry.hpp"

namespace Trident::DX12 {
	class AccelerationStructureBuildTriangleGeometry final : public Trident::AccelerationStructureBuildTriangleGeometry {
		public:
			sequence<D3D12_RAYTRACING_GEOMETRY_DESC> GeometryDescs;
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS BuildInputs;
	};
}