#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class RenderPass {
		public:
			explicit RenderPass(const RenderPassDescriptor& descriptor) :
				subpasses(descriptor.subpasses)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				for (const ref<Subpass>& subpass : subpasses) {
					attachmentCount += subpass->getColorAttachmentCount() + subpass->getResolveAttachmentCount();

					if (subpass->hasDepthStencilAttachment()) {
						// TODO: +1 if depth stencil resolve attachment?
						attachmentCount += 1;
						anyDepthStencilSubpass = true;
					}
				}

				ENGINE_ASSERT(attachmentCount > 0, "A render pass must have at least 1 attachment.");

				uint32 lastAttachmentIndex = attachmentCount - 1;

				for (sint32 subpassIndex = static_cast<sint32>(subpasses.count) - 1; subpassIndex >= 0; subpassIndex -= 1) {
					const ref<Subpass>& subpass = subpasses[subpassIndex];

					if (subpass->hasDepthStencilAttachment()) {
						if (subpass->getDepthStencilAttachment().loadOperation == LoadOperation::CLEAR) {
							lastClearDepthStencilAttachmentIndex = lastAttachmentIndex;
							lastClearAttachmentIndex = lastAttachmentIndex;

							break;
						}

						lastAttachmentIndex -= 1;
					}

					for (sint32 colorAttachmentIndex = static_cast<sint32>(subpass->getColorAttachmentCount()) - 1; colorAttachmentIndex >= 0; colorAttachmentIndex -= 1) {
						if (subpass->getColorAttachments()[colorAttachmentIndex].loadOperation == LoadOperation::CLEAR) {
							lastClearColorAttachmentIndex = lastAttachmentIndex;
							lastClearAttachmentIndex = lastAttachmentIndex;

							break;
						}

						lastAttachmentIndex -= (subpass->getColorAttachmentCount() + subpass->getResolveAttachmentCount()) / subpass->getColorAttachmentCount();
					}
				}
			}

			virtual ~RenderPass() = default;

			uint32 getAttachmentCount() const {
				return attachmentCount;
			}

			uint32 getLastClearAttachmentIndex() const {
				return lastClearAttachmentIndex;
			}

			bool hasAnyDepthStencilSubpass() const {
				return anyDepthStencilSubpass;
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

			// Largest attachment index specifying a load operation (or stencil load operation, if the attachment has a depth/stencil format) of LoadOperation::CLEAR.
			uint32 lastClearAttachmentIndex = max<uint32>();

			// These are used by the OpenGL adapter.
			uint32 lastClearColorAttachmentIndex = max<uint32>();
			uint32 lastClearDepthStencilAttachmentIndex = max<uint32>();

			sequence<ref<Subpass>> subpasses;
			sequence<SubpassDependency> subpassDependencies;

		private:
			// True if any subpass has a depth stencil attachment.
			bool anyDepthStencilSubpass = false;
	};
}