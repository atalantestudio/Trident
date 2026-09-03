// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"

namespace Trident {
	struct AccelerationStructureTriangleGeometryDescriptor {
		GeometryFlags flags;
		BaseBufferView vertexView;
		VertexInputAttributeFormat vertexFormat;

		/// The stride must either be 2 (uint16) or 4 (uint32).
		BaseBufferView indexView;
	};

	struct AccelerationStructureBuildTriangleGeometryDescriptor {
		AccelerationStructureBuildFlags flags;
		view<AccelerationStructureTriangleGeometryDescriptor> geometries;
	};
}