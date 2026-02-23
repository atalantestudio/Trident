// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Instance.hpp"
#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/Adapter/Window/Win32/base.hpp"

namespace Trident {
	DX12::Surface::Surface(const Trident::Instance* instance) :
		Trident::Surface(instance)
	{}

	ref<Trident::Surface> DX12::createSurface(const Trident::Instance* instance, const Window* window) {
		const DX12::Instance* _instance = static_cast<const DX12::Instance*>(instance);
		HRESULT result;

		ref<DX12::Surface> surface = createRef<DX12::Surface>(instance);
		surface->window = static_cast<const Win32WindowData*>(window->data)->hWnd;

		{
			PROFILE_SCOPE("MakeWindowAssociation");

			result = _instance->factory->MakeWindowAssociation(surface->window, DXGI_MWA_NO_ALT_ENTER);
		}

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to make window association.");

		return surface;
	}

	void DX12::destroySurface(const Trident::Instance* instance, const Trident::Surface* surface) {}

	ImageFormat DX12::getSurfaceFormat(const Trident::Surface* surface) {
		// TODO: Return correct surface format.
		return ImageFormat::B8G8R8A8_UNORM;
	}
}