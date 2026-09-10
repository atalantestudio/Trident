// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/InstanceFlags.hpp"

namespace Trident {
	struct AccelerationStructureInstance {
		float32 transform[3][4];
		uint32 index : 24;
		uint32 mask : 8;
		uint32 shaderBindingTableRecordOffset : 24;
		InstanceFlags flags : 8;
		uint64 accelerationStructureAddress;
	};
}