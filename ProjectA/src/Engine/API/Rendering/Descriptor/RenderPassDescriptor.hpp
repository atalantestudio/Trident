#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		std::vector<ColorAttachment> colorAttachments;
		DepthStencilAttachment depthStencilAttachment;

		bool enableResolve;
		bool enableDepthStencil;

		// Sets the index of the attachment that will render to a swapchain image.
		uint32 presentAttachmentIndex = std::numeric_limits<uint32>::max();
	};
}