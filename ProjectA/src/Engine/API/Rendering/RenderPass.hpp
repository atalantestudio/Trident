#pragma once

#include "Engine/API/Rendering/Attachment.hpp"

namespace ProjectA {
	struct RenderPassDescriptor {
		chain<Attachment> colorAttachments;
		DepthStencilAttachment depthStencilAttachment;
	};

	class RenderPass {
		public:
			RenderPass(bool depthStencil) :
				depthStencil(depthStencil)
			{}

			virtual ~RenderPass() = default;

			virtual void setAttachmentImageView(uint32 attachmentIndex, const ref<ImageView>& imageView) = 0;

		public:
			// Indicates if the render pass has a depth stencil attachment.
			const bool depthStencil;
	};
}