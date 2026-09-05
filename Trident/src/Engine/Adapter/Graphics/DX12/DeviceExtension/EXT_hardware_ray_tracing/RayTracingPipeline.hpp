// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"

namespace Trident::DX12 {
	class RayTracingPipeline final : public Trident::RayTracingPipeline {
		public:
			sequence<LPCWSTR> exportNames;
			ID3D12StateObject* stateObject;
			ID3D12StateObjectProperties* stateObjectProperties;
			ID3D12RootSignature* rootSignature;
	};

	void destroyRayTracingPipeline(const Trident::Device* device, const Trident::RayTracingPipeline* pipeline);

	void getRayTracingShaderGroupHandles(const Trident::Device* device, const Trident::RayTracingPipeline* pipeline, range<uint32, uint32> groupRange, void* mappedRange);
}