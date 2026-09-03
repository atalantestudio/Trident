// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	RayTracingPipeline::~RayTracingPipeline() {
		device->adapter->destroyRayTracingPipelineImplementation(device, this);
	}
}