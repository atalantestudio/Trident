// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"

namespace Trident::DX12 {
	class AccelerationStructure final : public Trident::AccelerationStructure {
		public:
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC BuildDesc;
	};

	uint64 getAccelerationStructureDeviceAddress(const Trident::Device* device, const Trident::AccelerationStructure* accelerationStructure);
}