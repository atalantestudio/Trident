#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSurface.hpp"

namespace ProjectA {
	ImageFormat OpenGLSurface::getFormat() const {
		// TODO: Retrieve correct surface format.
		//       Detecting the channel order (RGBA/BGRA) seems complicated as Win32 doesn't give much info on that.
		return ImageFormat::R8G8B8A8_UNORM;
	}
}