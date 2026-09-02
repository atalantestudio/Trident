// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/DeviceExtension.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"

namespace Trident {
	template<typename CommandEncoder>
	class CommandEncoderDeviceExtension<EXT_hardware_ray_tracing, CommandEncoder> {
		public:
			void buildAccelerationStructures(view<ref<AccelerationStructure>> accelerationStructures, view<BaseBufferView> accelerationStructureViews, view<BaseBufferView> scratchViews);
	};
}

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/CommandEncoder.ipp"