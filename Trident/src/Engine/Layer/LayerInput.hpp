// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Image.hpp"
#include "Engine/API/Graphics/ImageView.hpp"

namespace Trident {
	// TODO(layer/input): Remove.
	struct LayerInput {
		ref<Image> swapchainImage;
		ref<ImageView> swapchainImageView;
	};
}