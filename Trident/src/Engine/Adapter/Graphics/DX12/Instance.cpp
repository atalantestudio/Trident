// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"

namespace Trident {
	DX12::Instance::Instance(const Trident::GraphicsAdapter* adapter) :
		Trident::Instance(adapter)
	{}

	ref<Trident::Instance> DX12::createInstance(const Trident::GraphicsAdapter* adapter, const InstanceDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<DX12::Instance> instance = allocateRef<DX12::Instance>(adapter);
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

	void DX12::destroyInstance(const Trident::Instance* instance) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const DX12::Instance* _instance = static_cast<const DX12::Instance*>(instance);

		_instance->factory->Release();
	}
}