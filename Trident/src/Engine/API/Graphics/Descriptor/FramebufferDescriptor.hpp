// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageView.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"

namespace Trident {
	struct FramebufferDescriptor {
		ref<RenderPass> renderPass;

		// TODO(graphics/framebuffer): Use a view instead.
		sequence<ref<ImageView>> imageViews;

		Extent2D extent;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}