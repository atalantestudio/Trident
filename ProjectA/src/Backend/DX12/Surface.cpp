// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/Surface.hpp"
#include "Backend/Win32/base.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::Surface::Surface(const DX12::Instance& instance, const Window& window) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			this->window = pointer_cast<const Win32WindowData*>(window.data)->hWnd;

			// Don't support fullscreen transitions.
			result = instance.factory->MakeWindowAssociation(this->window, DXGI_MWA_NO_ALT_ENTER);

			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to disable fullscreen transitions.");
		#else
			#error "Unsupported operating system."
		#endif
	}

	ImageFormat DX12::Surface::getFormat() const {
		// TODO: Return correct surface format.
		return ImageFormat::R8G8B8A8_UNORM;
	}
}