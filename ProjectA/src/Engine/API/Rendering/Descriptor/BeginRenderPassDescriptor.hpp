#pragma once

#include "Engine/API/Rendering/ClearValue.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	struct BeginRenderPassDescriptor {
		ref<RenderPass> renderPass;

		// TODO: Has this swapchain always been created with the render pass above?
		//       If so, it could be directly accessed from it.
		ref<Swapchain> swapchain;

		// 1 clear value per attachment.
		// Clear values are ignored for attachments without a LoadOperation::CLEAR.
		std::vector<ClearValue> clearValues;

		// Index of the swapchain image to render to.
		uint32 imageIndex = 0;
	};
}