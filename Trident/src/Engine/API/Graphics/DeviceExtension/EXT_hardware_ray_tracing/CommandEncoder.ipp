// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Engine/API/Graphics/PhysicalDeviceLimits.hpp"

namespace Trident {
	template<typename CommandEncoder>
	void CommandEncoderDeviceExtension<EXT_hardware_ray_tracing, CommandEncoder>::bindPipeline(ref<RayTracingPipeline> pipeline) {
		CommandEncoder* commandEncoder = static_cast<CommandEncoder*>(this);

		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (pipeline == nullptr) {
					VALIDATION_ERROR("pipeline must be a valid RayTracingPipeline object.");
				}
			}
		#endif

		commandEncoder->device->adapter->bindRayTracingPipelineImplementation(commandEncoder, pipeline);
	}

	template<typename CommandEncoder>
	void CommandEncoderDeviceExtension<EXT_hardware_ray_tracing, CommandEncoder>::buildAccelerationStructures(view<ref<AccelerationStructure>> accelerationStructures, view<BaseBufferView> scratchViews) {
		CommandEncoder* commandEncoder = static_cast<CommandEncoder*>(this);

		commandEncoder->device->adapter->buildAccelerationStructuresImplementation(commandEncoder, accelerationStructures, scratchViews);
	}

	template<typename CommandEncoder>
	void CommandEncoderDeviceExtension<EXT_hardware_ray_tracing, CommandEncoder>::traceRays(
		const DeviceAddressRegion& generationShaderBindingTable,
		const DeviceAddressRegion& missShaderBindingTable,
		const DeviceAddressRegion& hitShaderBindingTable,
		const DeviceAddressRegion& callableShaderBindingTable,
		Extent3D extent
	) {
		CommandEncoder* commandEncoder = static_cast<CommandEncoder*>(this);

		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				const PhysicalDeviceLimits& limits = commandEncoder->device->getPhysicalDevice()->getLimits();
				const Extent3D maxExtent = limits.maxComputeWorkGroupSize * limits.maxComputeWorkGroupCount;

				Validator validator;

				if (extent.x > maxExtent.x) {
					VALIDATION_ERROR("descriptor.extent.x must not exceed maxComputeWorkGroupSize.x * maxComputeWorkGroupCount.x ([]).", maxExtent.x);
				}

				if (extent.y > maxExtent.y) {
					VALIDATION_ERROR("descriptor.extent.y must not exceed maxComputeWorkGroupSize.y * maxComputeWorkGroupCount.y ([]).", maxExtent.y);
				}

				if (extent.z > maxExtent.z) {
					VALIDATION_ERROR("descriptor.extent.z must not exceed maxComputeWorkGroupSize.z * maxComputeWorkGroupCount.z ([]).", maxExtent.z);
				}

				if (extent.x * extent.y * extent.z > limits.maxRayDispatchInvocationCount) {
					VALIDATION_ERROR("descriptor.extent.x * descriptor.extent.y * descriptor.extent.z must not exceed maxRayDispatchInvocationCount ([]).", limits.maxRayDispatchInvocationCount);
				}
			}
		#endif

		commandEncoder->device->adapter->traceRaysImplementation(commandEncoder, generationShaderBindingTable, missShaderBindingTable, hitShaderBindingTable, callableShaderBindingTable, extent);
	}
}