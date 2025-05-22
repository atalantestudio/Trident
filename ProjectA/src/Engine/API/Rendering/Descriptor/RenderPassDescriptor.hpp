#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		chain<ColorAttachment> colorAttachments;
		bool enableResolve;
		uint32 presentAttachmentIndex = std::numeric_limits<uint32>::max();
		DepthStencilAttachment depthStencilAttachment;
	};
}