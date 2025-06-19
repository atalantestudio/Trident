#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"

namespace ProjectA {
	struct ImageFile {
		ImageFormat format;

		// Only a depth of 1 is supported.
		Extent3D extent;

		sequence<uint8> data;
	};
}