// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Surface.hpp"
#include "Engine/API/Window/Window.hpp"

namespace Trident {
	class DX12::Surface final : public Trident::Surface {
		public:
			explicit Surface(const Trident::Instance* instance);

		public:
			HWND window;
	};

	namespace DX12 {
		ref<Trident::Surface> createSurface(const Trident::Instance* instance, ref<Trident::PhysicalDevice> physicalDevice, const Window* window);

		void destroySurface(const Trident::Instance* instance, const Trident::Surface* surface);
	}
}