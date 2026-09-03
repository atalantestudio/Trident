// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildGeometry.hpp"

namespace Trident {
	struct AccelerationStructureDescriptor {
		BaseBufferView view;
		ref<AccelerationStructureBuildGeometry> buildGeometry;
	};
}