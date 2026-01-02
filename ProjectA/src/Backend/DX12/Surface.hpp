// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Backend/DX12/Instance.hpp"

#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	class DX12::Surface : public ProjectA::Surface {
		public:
			explicit Surface(const DX12::Instance& instance, const Window& window);

			ImageFormat getFormat() const final override;

		public:
			HWND window;
	};
}