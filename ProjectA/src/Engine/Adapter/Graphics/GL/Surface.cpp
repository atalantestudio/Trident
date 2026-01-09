// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Surface.hpp"

namespace ProjectA {
	ImageFormat GL::Surface::getFormat() const {
		// TODO
		// Retrieve correct surface format.
		// Detecting the channel order (RGBA/BGRA) seems complicated as Win32 doesn't give much info on that.
		return ImageFormat::R8G8B8A8_UNORM;
	}
}