// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Surface.hpp"

namespace Trident {
	struct SwapchainDescriptor {
		ref<Surface> surface;
		uint32 imageCount;
		Extent2D extent;
		bool verticalSync;
	};
}