// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace Trident {
	struct FramebufferDescriptor {
		ref<RenderPass> renderPass;
		sequence<ref<ImageView>> imageViews;
		Extent2D extent;

		#if defined(BUILD_MODE_DEBUG)
			view<char8> name;
		#endif
	};
}