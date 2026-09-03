// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident::VK {
	class RayTracingPipeline final : public Trident::RayTracingPipeline {};

	void destroyRayTracingPipeline(const Trident::Device* device, const Trident::RayTracingPipeline* rayTracingPipeline);
}