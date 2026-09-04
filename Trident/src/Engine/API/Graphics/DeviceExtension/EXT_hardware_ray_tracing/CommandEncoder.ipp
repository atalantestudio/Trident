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
	void CommandEncoderDeviceExtension<EXT_hardware_ray_tracing, CommandEncoder>::buildAccelerationStructures(view<ref<AccelerationStructure>> accelerationStructures, view<BaseBufferView> accelerationStructureViews, view<BaseBufferView> scratchViews) {
		CommandEncoder* commandEncoder = static_cast<CommandEncoder*>(this);

		commandEncoder->device->adapter->buildAccelerationStructuresImplementation(commandEncoder, accelerationStructures, accelerationStructureViews, scratchViews);
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

				// TODO(lyah/vec3-uint32/operator*)
				vec3<uint32> size;
				size[0] = limits.maxComputeWorkGroupSize[0] * limits.maxComputeWorkGroupCount[0];
				size[0] = limits.maxComputeWorkGroupSize[1] * limits.maxComputeWorkGroupCount[1];
				size[0] = limits.maxComputeWorkGroupSize[2] * limits.maxComputeWorkGroupCount[2];

				Validator validator;

				if (extent.width > size[0]) {
					VALIDATION_ERROR("descriptor.extent.width must not exceed maxComputeWorkGroupSize[0] * maxComputeWorkGroupCount[0] ([]).", size[0]);
				}

				if (extent.height > size[1]) {
					VALIDATION_ERROR("descriptor.extent.height must not exceed maxComputeWorkGroupSize[1] * maxComputeWorkGroupCount[1] ([]).", size[1]);
				}

				if (extent.depth > size[2]) {
					VALIDATION_ERROR("descriptor.extent.depth must not exceed maxComputeWorkGroupSize[2] * maxComputeWorkGroupCount[2] ([]).", size[2]);
				}

				if (extent.width * extent.height * extent.depth > limits.maxRayDispatchInvocationCount) {
					VALIDATION_ERROR("descriptor.extent.width * descriptor.extent.height * descriptor.extent.depth must not exceed maxRayDispatchInvocationCount ([]).", limits.maxRayDispatchInvocationCount);
				}
			}
		#endif

		commandEncoder->device->adapter->traceRaysImplementation(commandEncoder, generationShaderBindingTable, missShaderBindingTable, hitShaderBindingTable, callableShaderBindingTable, extent);
	}
}