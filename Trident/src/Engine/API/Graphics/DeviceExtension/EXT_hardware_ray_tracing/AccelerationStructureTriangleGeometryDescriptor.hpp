// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"

namespace Trident {
	struct AccelerationStructureTriangleGeometryVertexDescriptor {
		ref<Buffer> buffer;
		VertexInputAttributeFormat format;
		uint64 stride;
		uint32 count;
	};

	struct AccelerationStructureTriangleGeometryIndexDescriptor {
		ref<Buffer> buffer;

		// 2 or 4 bytes.
		uint8 byteSize;

		uint32 count;
	};

	struct AccelerationStructureTriangleGeometryDescriptor {
		GeometryFlags flags;
		AccelerationStructureTriangleGeometryVertexDescriptor vertex;
		AccelerationStructureTriangleGeometryIndexDescriptor index;
	};
}