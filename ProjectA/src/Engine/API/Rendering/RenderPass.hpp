#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class RenderPass {
		public:
			RenderPass::RenderPass(const RenderPassDescriptor& descriptor) :
				subpasses(descriptor.subpasses)
			{
				DEBUG_VALIDATE_DESCRIPTOR(descriptor);

				for (const ref<Subpass>& subpass : descriptor.subpasses) {
					attachmentCount += subpass->getColorAttachmentCount() + subpass->getResolveAttachmentCount();

					if (subpass->hasDepthStencilAttachment()) {
						// TODO: +1 if depth stencil resolve attachment?
						attachmentCount += 1;
						depthStencilSubpass = true;
					}
				}

				DEBUG_ASSERT(attachmentCount > 0);

				// TODO: Restore clear values.
				/* if (depthStencil && depthStencilAttachment.loadOperation == LoadOperation::CLEAR) {
					clearValueCount += 1;
				}
				else {
					for (sint32 colorAttachmentIndex = static_cast<sint32>(colorAttachmentCount - 1); colorAttachmentIndex >= 0; colorAttachmentIndex -= 1) {
						const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

						if (colorAttachment.loadOperation != LoadOperation::CLEAR) {
							clearValueCount = colorAttachmentIndex;

							break;
						}
					}
				} */
			}

			virtual ~RenderPass() = default;

			uint32 getAttachmentCount() const {
				return attachmentCount;
			}

			uint32 getClearValueCount() const {
				return clearValueCount;
			}

			bool hasDepthStencilSubpass() const {
				return depthStencilSubpass;
			}

			const ref<Subpass>& getSubpass(uint32 index) const {
				ENGINE_ASSERT(index < subpasses.count, "Subpass index overflow.");

				return subpasses[index];
			}

			uint32 getSubpassCount() const {
				return static_cast<uint32>(subpasses.count);
			}

		protected:
			uint32 attachmentCount = 0;
			uint32 clearValueCount = 0;

		private:
			bool depthStencilSubpass = false;
			sequence<ref<Subpass>> subpasses;
	};
}