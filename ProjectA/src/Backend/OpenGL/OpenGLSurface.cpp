#include "pch.hpp"

#include "Backend/OpenGL/OpenGLSurface.hpp"

namespace ProjectA {
	ImageFormat OpenGLSurface::getFormat() const {
		// TODO: Retrieve correct surface format.
		return ImageFormat::R8G8B8A8_UNORM;
	}
}