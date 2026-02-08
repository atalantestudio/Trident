// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class RenderPass {
		public:
			explicit RenderPass(const RenderPassDescriptor& descriptor) :
				attachments(descriptor.attachments),
				subpasses(descriptor.subpasses)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				for (ref<Subpass> subpass : subpasses) {
					uint8 rasterizationSampleCount = 0;

					for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
						if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
							continue;
						}

						const Attachment& attachment = attachments[colorAttachmentReference.index];

						if (attachment.sampleCount > 1) {
							attachmentReferenceCount += 1;
						}

						if (rasterizationSampleCount == 0) {
							rasterizationSampleCount = attachment.sampleCount;

							continue;
						}

						// TODO: There is no need for each attachment to have a sampleCount member.
						ENGINE_ASSERT(rasterizationSampleCount == attachment.sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");
					}

					if (subpass->getDepthStencilAttachmentReference().index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();

						// TODO: There is no need for each attachment to have a sampleCount member.
						ENGINE_ASSERT(rasterizationSampleCount == attachments[depthStencilAttachmentReference.index].sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");

						attachmentReferenceCount += 1;
					}

					DEBUG_ASSERT(rasterizationSampleCount > 0);

					subpass->setRasterizationSampleCount(rasterizationSampleCount);

					attachmentReferenceCount += subpass->getColorAttachmentReferenceCount() + subpass->getInputAttachmentReferenceCount();
				}
			}

			virtual ~RenderPass() = default;

			// Returns the number of attachments references, including resolve references.
			uint32 getAttachmentReferenceCount() const {
				return static_cast<uint32>(attachmentReferenceCount);
			}

			uint32 getAttachmentCount() const {
				return static_cast<uint32>(attachments.count());
			}

			const Attachment& getAttachment(uint32 attachmentIndex) const {
				if (attachmentIndex >= attachments.count()) {
					DEBUG_ASSERT(attachmentIndex < attachmentReferenceCount);

					// `attachmentIndex` refers to a resolve attachment.
					// Return the corresponding color attachment.
					return attachments[attachmentIndex - attachments.count()];
				}

				return attachments[attachmentIndex];
			}

			uint32 getSubpassCount() const {
				return static_cast<uint32>(subpasses.count());
			}

			ref<Subpass> getSubpass(uint32 subpassIndex) const {
				DEBUG_ASSERT(subpassIndex < subpasses.count());

				return subpasses[subpassIndex];
			}

			ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const;

			ref<Image> createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 subpassIndex) const;

			ref<Image> createInputAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const;

		protected:
			sequence<Attachment> attachments;
			sequence<ref<Subpass>> subpasses;
			uint32 attachmentReferenceCount = 0;
	};
}