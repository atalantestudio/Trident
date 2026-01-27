// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	class DX12::Surface final : public Trident::Surface {
		public:
			explicit Surface(const Window& window);

			ImageFormat getFormat() const final override;

		public:
			HWND window;
	};
}