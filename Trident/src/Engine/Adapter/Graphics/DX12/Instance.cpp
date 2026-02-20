// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

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

		score += isIntegratedAdapter(adapterDescriptor) && !preferIntegratedGpu;

		return score;
	}

	DX12::Instance::Instance(const Trident::GraphicsAdapter* adapter) :
		Trident::Instance(adapter)
	{}

	ref<Trident::Instance> DX12::createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<DX12::Instance> instance = createRef<DX12::Instance>(adapter);
		HRESULT result;
		UINT flags = 0;

		#if defined(DEBUG_ENABLE_MESSENGER)
			ID3D12Debug* debug;

			{
				PROFILE_SCOPE("D3D12GetDebugInterface");

				result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
			}

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get debug interface.");

			debug->EnableDebugLayer();
			debug->Release();

			flags |= DXGI_CREATE_FACTORY_DEBUG;
		#endif

		{
			PROFILE_SCOPE("CreateDXGIFactory2");

			result = CreateDXGIFactory2(flags, IID_PPV_ARGS(&instance->factory));
		}

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create factory.");

		return instance;
	}

	ref<Trident::PhysicalDevice> DX12::selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu) {
		const DX12::Instance* _instance = static_cast<const DX12::Instance*>(instance);
		HRESULT result;
		UINT adapterIndex = 0;
		UINT bestAdapterScore = 0;
		UINT bestAdapterIndex = 0;
		IDXGIAdapter* adapter;
		std::vector<IDXGIAdapter*> adapters;
		DXGI_ADAPTER_DESC adapterDescriptor;

		while (true) {
			result = _instance->factory->EnumAdapters(adapterIndex, &adapter);

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

			const uint32 adapterScore = calculateAdapterScore(adapterDescriptor, preferIntegratedGpu);

			if (adapterScore > bestAdapterScore) {
				bestAdapterScore = adapterScore;
				bestAdapterIndex = adapterIndex;
			}

			const LPCSTR description = ConvertUTF16ToUTF8(adapterDescriptor.Description);

			ADAPTER_INFO("- []", description);

			delete[] description;

			adapterIndex += 1;
		}

		adapter = adapters[bestAdapterIndex];

		result = adapter->GetDesc(&adapterDescriptor);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get adapter descriptor.");

		const LPCSTR description = ConvertUTF16ToUTF8(adapterDescriptor.Description);

		ADAPTER_INFO("Using [].", description);

		delete[] description;

		return createRef<DX12::PhysicalDevice>(_instance->adapter, adapter, _instance->factory);
	}

	void DX12::destroyInstance(const Trident::Instance* instance) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Instance* _instance = static_cast<const DX12::Instance*>(instance);

		_instance->factory->Release();
	}
}