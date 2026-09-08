// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/DeviceAddressRegion.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident {
	// TODO(graphics/ext_hardware_ray_tracing): Ray tracing command encoders must have been created by a queue that supports compute operations.
	template<typename CommandEncoder>
	class CommandEncoderDeviceExtension<EXT_hardware_ray_tracing, CommandEncoder> {
		public:
			void bindPipeline(ref<RayTracingPipeline> pipeline);

			void buildAccelerationStructures(view<ref<AccelerationStructure>> accelerationStructures, view<BaseBufferView> scratchViews);

			/// Initializes a ray tracing dispatch.
			void traceRays(
				/// The shader binding table for the ray generation stage.
				const DeviceAddressRegion& generationShaderBindingTable,

				/// The shader binding table for the ray miss stage.
				const DeviceAddressRegion& missShaderBindingTable,

				/// The shader binding table for the ray hit stage.
				const DeviceAddressRegion& hitShaderBindingTable,

				/// The shader binding table for the ray callable stage.
				const DeviceAddressRegion& callableShaderBindingTable,

				/// Size of the dispatch.
				Extent3D extent
			);

			// TODO(graphics/ext_hardware_ray_tracing): Add indirect ray tracing support.
	};
}

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/CommandEncoder.ipp"