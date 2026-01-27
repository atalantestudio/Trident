#pragma once

#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace Trident {
	struct FramebufferDescriptor {
		ref<RenderPass> renderPass;
		sequence<ref<ImageView>> imageViews;
		Extent2D extent;
		uint32 layerCount;
	};
}