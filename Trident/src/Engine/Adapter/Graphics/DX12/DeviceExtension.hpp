// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/API/Graphics/DeviceExtension/DeviceExtensionName.hpp"

namespace Trident::DX12 {
	inline void enableHardwareRayTracingDeviceExtension(DX12::Device* device) {
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureData{};
		HRESULT result;

		result = device->device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureData, sizeof(featureData));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to check feature support for EXT_hardware_ray_tracing.");
		ADAPTER_ASSERT(featureData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED, "EXT_hardware_ray_tracing is not supported on this device.");

		result = device->device->QueryInterface(&device->device5);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to query ID3D12Device5 interface.");
	}

	inline void enableDeviceExtension(DeviceExtensionName extension, DX12::Device* device) {
		switch (extension) {
			case EXT_hardware_ray_tracing: {
				enableHardwareRayTracingDeviceExtension(device);

				break;
			}
		}
	}
}