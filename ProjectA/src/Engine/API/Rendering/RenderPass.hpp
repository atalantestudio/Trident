#pragma once

#include "Engine/API/Rendering/Descriptor/RenderPassDescriptor.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class RenderPass {
		public:
			explicit RenderPass(const RenderPassDescriptor& descriptor) :
				attachments(descriptor.attachments),
				subpasses(descriptor.subpasses)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);

				for (ref<Subpass>& subpass : subpasses) {
					uint8 rasterizationSampleCount = 0;

					for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
						if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
							continue;
						}

						if (rasterizationSampleCount == 0) {
							rasterizationSampleCount = attachments[colorAttachmentReference.index].sampleCount;

							continue;
						}

						ENGINE_ASSERT(rasterizationSampleCount == attachments[colorAttachmentReference.index].sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");
					}

					if (subpass->getDepthStencilAttachmentReference().index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						const AttachmentReference& depthStencilAttachmentReference = subpass->getDepthStencilAttachmentReference();

						ENGINE_ASSERT(rasterizationSampleCount == attachments[depthStencilAttachmentReference.index].sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");

						attachmentReferenceCount += 1;
					}

					DEBUG_ASSERT(rasterizationSampleCount > 0);

					subpass->setRasterizationSampleCount(rasterizationSampleCount);

					attachmentReferenceCount += subpass->getColorAttachmentReferenceCount() + subpass->getInputAttachmentReferenceCount();

					// If the subpass uses multisample, add resolve attachments to the total reference count.
					if (rasterizationSampleCount > 1) {
						attachmentReferenceCount += subpass->getColorAttachmentReferenceCount();
					}
				}
			}

			virtual ~RenderPass() = default;

			uint32 getAttachmentCount() const {
				return static_cast<uint32>(attachments.count);
			}

			uint32 getSubpassCount() const {
				return static_cast<uint32>(subpasses.count);
			}

			const ref<Subpass>& getSubpass(uint32 subpassIndex) const {
				ENGINE_ASSERT(subpassIndex < subpasses.count, "Subpass index overflow.");

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