#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		chain<Attachment> colorAttachments;
		Attachment depthStencilAttachment;

		// If a depth stencil attachment is provided, then `enableDepthStencil` must be true.
		bool enableDepthStencil = false;
	};

	class RenderPass {
		public:
			RenderPass(bool depthStencil) :
				depthStencil(depthStencil)
			{}

			virtual ~RenderPass() = default;

		public:
			// Indicates if the render pass has a depth stencil attachment.
			const bool depthStencil;
	};
}