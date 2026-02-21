// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/Adapter/Window/Win32/base.hpp"

namespace Trident {
	DX12::Surface::Surface(const Trident::Instance* instance, HWND window) :
		Trident::Surface(instance),
		window(window)
	{}

	ref<Trident::Surface> DX12::createSurface(const Trident::Instance* instance, const Window* window) {
		return createRef<DX12::Surface>(instance, static_cast<const Win32WindowData*>(window->data)->hWnd);
	}

	void DX12::destroySurface(const Trident::Instance* instance, const Trident::Surface* surface) {}

	ImageFormat DX12::getSurfaceFormat(const Trident::Surface* surface) {
		// TODO: Return correct surface format.
		return ImageFormat::B8G8R8A8_UNORM;
	}
}