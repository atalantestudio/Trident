#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"

namespace ProjectA {
	class RenderPass {
		public:
			RenderPass(const RenderPassDescriptor& descriptor) :
				depthStencil(descriptor.depthStencilAttachment.enabled),
				colorAttachmentCount(static_cast<uint32>(descriptor.colorAttachments.size())),
				resolveAttachmentCount(descriptor.enableResolve ? colorAttachmentCount : 0),
				presentAttachmentIndex(descriptor.presentAttachmentIndex)
			{
				attachmentCount = colorAttachmentCount + resolveAttachmentCount;

				if (depthStencil) {
					attachmentCount += 1;
				}
			}

			virtual ~RenderPass() = default;

			uint32 getAttachmentCount() const {
				return attachmentCount;
			}

			uint32 getColorAttachmentCount() const {
				return colorAttachmentCount;
			}

			uint32 getResolveAttachmentCount() const {
				return resolveAttachmentCount;
			}

			uint32 getPresentAttachmentIndex() const {
				return presentAttachmentIndex;
			}

		public:
			const bool depthStencil;

		protected:
			uint32 attachmentCount;
			uint32 colorAttachmentCount;
			uint32 resolveAttachmentCount;
			uint32 presentAttachmentIndex;
	};
}