// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"

namespace Trident {
	struct AccelerationStructureBuildInstanceGeometryDescriptor {
		AccelerationStructureBuildFlags flags;
		GeometryFlags geometryFlags;
		BaseBufferView instanceView;
		uint32 instanceCount;
	};
}