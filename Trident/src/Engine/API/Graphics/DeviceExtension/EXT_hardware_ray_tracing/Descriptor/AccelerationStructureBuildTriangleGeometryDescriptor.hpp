// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/API/Graphics/IndexFormat.hpp"
#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"

namespace Trident {
	struct AccelerationStructureTriangleGeometryDescriptor {
		GeometryFlags flags;
		BaseBufferView vertexView;
		VertexInputAttributeFormat vertexFormat;
		BaseBufferView indexView;
		IndexFormat indexFormat;
	};

	struct AccelerationStructureBuildTriangleGeometryDescriptor {
		AccelerationStructureBuildFlags flags;
		view<AccelerationStructureTriangleGeometryDescriptor> geometries;
	};
}