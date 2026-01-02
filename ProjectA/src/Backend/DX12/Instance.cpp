// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/Instance.hpp"

namespace ProjectA {
	DX12::Instance::Instance() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		UINT flags = 0;

		#ifdef BUILD_MODE_DEBUG
			ComPtr<ID3D12Debug> debug;

			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
				flags |= DXGI_CREATE_FACTORY_DEBUG;

				debug->EnableDebugLayer();
			}
		#endif

		ENGINE_ASSERT(SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory))), "Could not create DXGI factory.");
	}
}