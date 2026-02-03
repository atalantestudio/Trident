// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Rendering/RenderPass.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	ref<Image> RenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);
		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		DEBUG_ASSERT(attachment.sampleCount > 1 || (attachment.sampleCount == 1 && attachment.finalLayout != ImageLayout::PRESENT_SRC));

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = attachment.format;
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = attachment.sampleCount;
		imageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT | descriptor.additionalUsage;
		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		#if defined(BUILD_MODE_DEBUG)
			imageDescriptor.name = descriptor.name;
		#endif

		return Renderer::createImage(imageDescriptor);
	}

	ref<Image> RenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 subpassIndex) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);
		DEBUG_ASSERT(subpassIndex < subpasses.count());

		const ref<Subpass> subpass = subpasses[subpassIndex];

		ENGINE_ASSERT(subpass->hasDepthStencilAttachmentReference(), "The subpass doesn't use a depth/stencil attachment.");

		const uint32 attachmentIndex = subpass->getDepthStencilAttachmentReference().index;

		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = static_cast<ImageFormat>(attachment.format);
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = attachment.sampleCount;
		imageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT | descriptor.additionalUsage;

		bool isInputAttachment = false;

		for (uint32 nextSubpassIndex = subpassIndex; nextSubpassIndex < subpasses.count(); nextSubpassIndex += 1) {
			const ref<Subpass> subpass = subpasses[nextSubpassIndex];

			for (const AttachmentReference& inputAttachmentReference : subpass->getInputAttachmentReferences()) {
				if (inputAttachmentReference.index == attachmentIndex) {
					isInputAttachment = true;

					break;
				}
			}

			if (isInputAttachment) {
				break;
			}
		}

		if (isInputAttachment) {
			imageDescriptor.usage |= ImageUsage::INPUT_ATTACHMENT;
		}

		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		#if defined(BUILD_MODE_DEBUG)
			imageDescriptor.name = descriptor.name;
		#endif

		return Renderer::createImage(imageDescriptor);
	}

	ref<Image> RenderPass::createInputAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);
		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = attachment.format;
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = attachment.sampleCount;
		imageDescriptor.usage = ImageUsage::INPUT_ATTACHMENT | descriptor.additionalUsage;

		if (isDepthFormat(attachment.format)) {
			imageDescriptor.usage |= ImageUsage::DEPTH_STENCIL_ATTACHMENT;
		}
		else {
			imageDescriptor.usage |= ImageUsage::COLOR_ATTACHMENT;
		}

		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		#if defined(BUILD_MODE_DEBUG)
			imageDescriptor.name = descriptor.name;
		#endif

		return Renderer::createImage(imageDescriptor);
	}
}