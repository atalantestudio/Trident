// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Surface.hpp"
#include "Engine/Adapter/Window/Win32/base.hpp"

namespace Trident {
	DX12::Surface::Surface(const DX12::Instance* instance, const Window* window) :
		window(static_cast<const Win32WindowData*>(window->data)->hWnd)
	{}

	ImageFormat DX12::Surface::getFormat() const {
		// TODO: Return correct surface format.
		return ImageFormat::B8G8R8A8_UNORM;
	}
}