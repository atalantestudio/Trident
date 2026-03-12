// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Surface.hpp"

namespace Trident {
	class GL::Surface final : public Trident::Surface {
		public:
			explicit Surface(const Trident::Instance* instance);

		public:
			HDC deviceContext;
			HGLRC renderingContext;
			GL::PFNWGLDELETECONTEXTPROC wglDeleteContext;
			GL::PFNWGLMAKECURRENTPROC wglMakeCurrent;
	};

	namespace GL {
		ref<Trident::Surface> createSurface(const Trident::Instance* instance, ref<Trident::PhysicalDevice> physicalDevice, const Window* window);

		void destroySurface(const Trident::Instance* instance, const Trident::Surface* surface);
	}
}