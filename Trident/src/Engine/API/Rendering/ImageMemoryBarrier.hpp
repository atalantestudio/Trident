// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Access.hpp"
#include "Engine/API/Rendering/Image.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"

namespace Trident {
	struct ImageMemoryBarrier {
		ref<Image> image;
		ImageLayout newLayout;

		// TODO: Store inside the image.
		Access sourceAccess;

		Access destinationAccess;
	};
}