// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/EXT_hardware_ray_tracing.hpp"

namespace Trident::DX12 {
	void buildAccelerationStructures(Trident::CommandEncoder* commandEncoder, view<ref<Trident::AccelerationStructure>> accelerationStructures, view<BaseBufferView> accelerationStructureViews, view<BaseBufferView> scratchViews);
}