// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Descriptor/AttachmentImageDescriptor.hpp"
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

			const sequence<Subpass>& getSubpasses() const;

			const Subpass& getSubpass(uint32 subpassIndex) const;

			ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const;

			ref<Image> createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 subpassIndex) const;

			ref<Image> createInputAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const;

		public:
			sequence<Attachment> attachments;
			uint32 attachmentReferenceCount = 0;

		protected:
			const Device* device;

		private:
			sequence<Subpass> subpasses;
	};
}