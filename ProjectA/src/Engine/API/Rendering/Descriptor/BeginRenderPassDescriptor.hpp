#pragma once

#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	struct BeginRenderPassDescriptor {
		ref<RenderPass> renderPass;

		// TODO: Has this swapchain always been created with the render pass above?
		//       If so, it could be directly accessed from it.
		ref<Swapchain> swapchain;

		// A clear value color each color attachment.
		chain<vec4<float32>> clearValues;

		// Index of the swapchain image to render to.
		uint32 imageIndex = 0;
	};
}