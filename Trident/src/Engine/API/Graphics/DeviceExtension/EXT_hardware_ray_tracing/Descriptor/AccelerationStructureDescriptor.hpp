// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildTriangleGeometry.hpp"

namespace Trident {
	struct AccelerationStructureDescriptor {
		BaseBufferView view;
		ref<AccelerationStructureBuildTriangleGeometry> buildTriangleGeometry;
	};
}