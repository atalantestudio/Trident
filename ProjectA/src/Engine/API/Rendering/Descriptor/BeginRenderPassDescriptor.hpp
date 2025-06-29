#pragma once

#include "Engine/API/Rendering/ClearValue.hpp"
#include "Engine/API/Rendering/Framebuffer.hpp"

namespace ProjectA {
	struct BeginRenderPassDescriptor {
		ref<Framebuffer> framebuffer;

		// TODO: Use sequence<ClearValue>.
		// 1 clear value per attachment.
		// This list must be as large as N = 1 + the index of the last attachment in the render pass using a load operation (or stencil load operation) of LoadOperation::CLEAR.
		// Clear values are ignored for the attachments in between the range [0, N] which use other load operations.
		std::vector<ClearValue> clearValues;
	};
}