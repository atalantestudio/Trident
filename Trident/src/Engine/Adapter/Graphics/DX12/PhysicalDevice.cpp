// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"

namespace Trident {
	static bool isIntegratedAdapter(const DXGI_ADAPTER_DESC& adapterDescriptor) {
		// TODO: Use a view instead.
		static constexpr UINT SEARCH_COUNT = 13;
		static constexpr WCHAR SEARCH[SEARCH_COUNT] = L"UHD Graphics";

		const WCHAR* iterator = std::search(adapterDescriptor.Description, adapterDescriptor.Description + 127, SEARCH, SEARCH + SEARCH_COUNT - 1);

		return iterator != adapterDescriptor.Description + 127;
	}

	static uint32 calculateAdapterScore(const DXGI_ADAPTER_DESC& adapterDescriptor, bool preferIntegratedGpu) {
		uint32 score = 1;

		if (isIntegratedAdapter(adapterDescriptor) && preferIntegratedGpu) {
			score += 1;
		}
		else if (!isIntegratedAdapter(adapterDescriptor) && !preferIntegratedGpu) {
			score += 1;
		}

		return score;
	}

	DX12::PhysicalDevice::PhysicalDevice(const Trident::GraphicsAdapter* adapter, IDXGIAdapter* dxgiAdapter, const DXGI_ADAPTER_DESC& adapterDescriptor, IDXGIFactory4* factory) :
		Trident::PhysicalDevice(adapter),
		adapter(dxgiAdapter),
		adapterDescriptor(adapterDescriptor),
		factory(factory),
		name(ConvertUTF16ToUTF8(adapterDescriptor.Description))
	{
		static constexpr uint32 D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION = 65535;

		limits.maxComputeWorkgroupCount[0] = D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
		limits.maxComputeWorkgroupCount[1] = D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
		limits.maxComputeWorkgroupCount[2] = D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
		limits.minStorageBufferOffsetAlignment = D3D12_RAW_UAV_SRV_BYTE_ALIGNMENT;
		limits.minUniformBufferOffsetAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	}

	ref<Trident::PhysicalDevice> DX12::selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu) {
		const DX12::Instance* _instance = static_cast<const DX12::Instance*>(instance);
		HRESULT result;
		UINT adapterIndex = 0;
		IDXGIAdapter* adapter;
		DXGI_ADAPTER_DESC adapterDescriptor;
		UINT bestAdapterScore = 0;
		UINT bestAdapterIndex = 0;
		DXGI_ADAPTER_DESC bestAdapterDescriptor;

		ADAPTER_INFO("Available adapters:");

		while (true) {
			result = _instance->factory->EnumAdapters(adapterIndex, &adapter);

			if (result == DXGI_ERROR_NOT_FOUND) {
				break;
			}

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate adapter.");

			result = adapter->GetDesc(&adapterDescriptor);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get adapter descriptor.");

			const uint32 adapterScore = calculateAdapterScore(adapterDescriptor, preferIntegratedGpu);

			if (adapterScore > bestAdapterScore) {
				bestAdapterScore = adapterScore;
				bestAdapterIndex = adapterIndex;
				bestAdapterDescriptor = adapterDescriptor;
			}

			const LPCSTR description = ConvertUTF16ToUTF8(adapterDescriptor.Description);

			ADAPTER_INFO("- []", description);

			delete[] description;

			adapter->Release();

			adapterIndex += 1;
		}

		result = _instance->factory->EnumAdapters(bestAdapterIndex, &adapter);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate adapter.");

		return createRef<DX12::PhysicalDevice>(_instance->adapter, adapter, bestAdapterDescriptor, _instance->factory);
	}

	uint32 DX12::getPhysicalDeviceMemoryTypeCountImplementation(const Trident::PhysicalDevice* physicalDevice) {
		// Count DEFAULT, UPLOAD and READBACK.
		return 3;
	}

	view<char8> DX12::getPhysicalDeviceName(const Trident::PhysicalDevice* physicalDevice) {
		return static_cast<const DX12::PhysicalDevice*>(physicalDevice)->name;
	}
}