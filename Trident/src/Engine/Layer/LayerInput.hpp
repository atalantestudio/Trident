// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Image.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace Trident {
	struct LayerInput {
		ref<Image> swapchainImage;
		ref<ImageView> swapchainImageView;
	};
}