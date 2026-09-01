// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/EXT_hardware_ray_tracing.hpp"

namespace Trident::DX12 {
	AccelerationStructureBuildSizes getAccelerationStructureBuildSizesImplementation(const Trident::Device* device, const AccelerationStructureBuildGeometryDescriptor& descriptor);
}