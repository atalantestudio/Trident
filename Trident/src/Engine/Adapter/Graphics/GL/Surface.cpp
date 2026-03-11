// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Surface.hpp"

namespace Trident {
	GL::Surface::Surface(const Trident::Instance* instance) :
		Trident::Surface(instance)
	{}

	ref<Trident::Surface> GL::createSurface(const Trident::Instance* instance, ref<Trident::PhysicalDevice> physicalDevice, const Window* window) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Surface> surface = createRef<GL::Surface>(instance);

		// TODO: Retrieve correct surface format.
		// Detecting the channel order (RGBA/BGRA) seems complicated as Win32 doesn't give much info on that.
		surface->format = ImageFormat::B8G8R8A8_UNORM;

		// TODO: Get default framebuffer color read format (helps to choose between RGBA and BGRA):
		// glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &format);

		// TODO: To get the bit count of each channel of the default framebuffer format, call glGetIntegerv with:
		// - GL_RED_BITS
		// - GL_GREEN_BITS
		// - GL_BLUE_BITS
		// - GL_ALPHA_BITS
		// - GL_DEPTH_BITS
		// - GL_STENCIL_BITS

		return surface;
	}

	void GL::destroySurface(const Trident::Instance* instance, const Trident::Surface* surface) {}
}