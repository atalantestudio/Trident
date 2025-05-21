#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		chain<Attachment> colorAttachments;
		scope<Attachment> depthStencilAttachment = nullptr;
	};

	class RenderPass {
		public:
			virtual ~RenderPass() = default;
	};
}