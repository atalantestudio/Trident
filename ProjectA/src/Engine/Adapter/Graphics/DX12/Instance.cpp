// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Instance::Instance(const ApplicationProperties& properties) :
		preferIntegratedGpu(properties.PreferIntegratedGPU)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;
		UINT flags = 0;

		#ifdef BUILD_MODE_DEBUG
			{
				ComPtr<ID3D12Debug> debug;

				result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));

				ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get debug interface.");

				debug->EnableDebugLayer();
			}

			flags |= DXGI_CREATE_FACTORY_DEBUG;
		#endif

		result = CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create factory.");
	}

	IDXGIAdapter* DX12::Instance::selectAdapter() const {
		HRESULT result;
		UINT adapterIndex = 0;
		UINT bestAdapterScore = 0;
		UINT bestAdapterIndex = 0;
		IDXGIAdapter* adapter;
		std::vector<IDXGIAdapter*> adapters;
		DXGI_ADAPTER_DESC adapterDescriptor;

		while (true) {
			result = factory->EnumAdapters(adapterIndex, &adapter);

			if (result == DXGI_ERROR_NOT_FOUND) {
				break;
			}

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to enumerate adapters.");

			adapters.emplace_back(adapter);

			adapterIndex += 1;
		}

		adapterIndex = 0;

		ADAPTER_INFO("Available adapters:");

		while (adapterIndex < adapters.size()) {
			IDXGIAdapter* adapter = adapters[adapterIndex];

			result = adapter->GetDesc(&adapterDescriptor);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get adapter descriptor.");

			const uint32 adapterScore = calculateAdapterScore(adapterDescriptor);

			if (adapterScore > bestAdapterScore) {
				bestAdapterScore = adapterScore;
				bestAdapterIndex = adapterIndex;
			}

			const LPCSTR description = ConvertUTF16ToUTF8(adapterDescriptor.Description);

			ADAPTER_INFO("- []", description);

			delete[] description;

			adapterIndex += 1;
		}

		return adapters[bestAdapterIndex];
	}

	bool DX12::Instance::isIntegratedAdapter(const DXGI_ADAPTER_DESC& adapterDescriptor) const {
		return std::wstring(adapterDescriptor.Description).find(L"UHD Graphics", 0) != std::string::npos;
	}

	uint32 DX12::Instance::calculateAdapterScore(const DXGI_ADAPTER_DESC& adapterDescriptor) const {
		uint32 score = 1;

		if (isIntegratedAdapter(adapterDescriptor)) {
			score += preferIntegratedGpu;
		}
		else {
			score += !preferIntegratedGpu;
		}

		return score;
	}
}