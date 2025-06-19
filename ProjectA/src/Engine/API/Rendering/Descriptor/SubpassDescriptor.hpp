#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct SubpassDescriptor {
		sequence<ColorAttachment> colorAttachments;

		// Sets the index of the attachment that will render to the swapchain image.
		uint32 presentAttachmentIndex = std::numeric_limits<uint32>::max();

		bool depthStencil;
		DepthStencilAttachment depthStencilAttachment;
	};
}