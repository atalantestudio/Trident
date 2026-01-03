// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Instance::Instance() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;
		UINT flags = 0;

		#ifdef BUILD_MODE_DEBUG
			ComPtr<ID3D12Debug> debug;

			result = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get debug controller.");

			flags |= DXGI_CREATE_FACTORY_DEBUG;

			debug->EnableDebugLayer();
		#endif

		result = CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create DXGI factory.");
	}
}