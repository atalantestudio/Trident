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

	void RayTracingPipeline::getShaderGroupHandles(range<uint32, uint32> groupRange, void* mappedRange) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (groupRange.count == 0) {
					VALIDATION_ERROR("descriptor.groupRange.count must be greater than 0.");
				}
			}
		#endif

		device->adapter->getRayTracingShaderGroupHandlesImplementation(device, this, groupRange, mappedRange);
	}
}