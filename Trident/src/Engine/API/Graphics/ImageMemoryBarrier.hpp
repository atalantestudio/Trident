// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/Image.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"

namespace Trident {
	struct ImageMemoryBarrier {
		ref<Image> image;
		ImageLayout newLayout;
		Access newAccess;
	};
}