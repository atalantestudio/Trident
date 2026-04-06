// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/RenderPassDescriptor.hpp"

namespace Trident {
	class RenderPass {
		public:
			explicit RenderPass(const Device* device, const RenderPassDescriptor& descriptor);

			~RenderPass();

			// Returns the number of attachments references, including resolve references.
			uint32 getAttachmentReferenceCount() const;

			uint32 getAttachmentCount() const;

			const Attachment& getAttachment(uint32 attachmentIndex) const;

			uint32 getSubpassCount() const;

			ref<Subpass> getSubpass(uint32 subpassIndex) const;

			ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const;

			ref<Image> createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 subpassIndex) const;

			ref<Image> createInputAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const;

		protected:
			const Device* device;

		public:
			sequence<Attachment> attachments;
			sequence<ref<Subpass>> subpasses;
			uint32 attachmentReferenceCount = 0;
	};
}