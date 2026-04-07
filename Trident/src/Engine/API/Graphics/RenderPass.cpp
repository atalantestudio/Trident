// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"

namespace Trident {
	#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
		static void validateAttachmentImageDescriptor(const AttachmentImageDescriptor& descriptor) {
			uint32 warningCount = 0;
			uint32 errorCount = 0;

			if (descriptor.type >= ImageType::count) {
				ENGINE_ERROR("descriptor.type must be a valid ImageType value.");

				errorCount += 1;
			}

			if (descriptor.extent.width == 0) {
				ENGINE_ERROR("descriptor.extent.width must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.extent.height == 0) {
				ENGINE_ERROR("descriptor.extent.height must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.extent.depth == 0) {
				ENGINE_ERROR("descriptor.extent.depth must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.mipLevelCount == 0) {
				ENGINE_ERROR("descriptor.mipLevelCount must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.layerCount == 0) {
				ENGINE_ERROR("descriptor.layerCount must be greater than 0.");

				errorCount += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		}
	#endif

	RenderPass::RenderPass(const Device* device, const RenderPassDescriptor& descriptor) :
		device(device),
		attachments(descriptor.attachments),
		subpasses(descriptor.subpasses)
	{
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

	RenderPass::~RenderPass() {
		device->adapter->destroyRenderPassImplementation(device, this);
	}

	uint32 RenderPass::getAttachmentReferenceCount() const {
		return static_cast<uint32>(attachmentReferenceCount);
	}

	uint32 RenderPass::getAttachmentCount() const {
		return static_cast<uint32>(attachments.count());
	}

	const Attachment& RenderPass::getAttachment(uint32 attachmentIndex) const {
		if (attachmentIndex >= attachments.count()) {
			DEBUG_ASSERT(attachmentIndex < attachmentReferenceCount);

			// `attachmentIndex` refers to a resolve attachment.
			// Return the corresponding color attachment.
			return attachments[attachmentIndex - attachments.count()];
		}

		return attachments[attachmentIndex];
	}

	uint32 RenderPass::getSubpassCount() const {
		return static_cast<uint32>(subpasses.count());
	}

	ref<Subpass> RenderPass::getSubpass(uint32 subpassIndex) const {
		DEBUG_ASSERT(subpassIndex < subpasses.count());

		return subpasses[subpassIndex];
	}

	ref<Image> RenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateAttachmentImageDescriptor(descriptor);
		#endif

		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		DEBUG_ASSERT(attachment.sampleCount > 1 || (attachment.sampleCount == 1 && attachment.finalLayout != ImageLayout::PRESENT_SRC));

		ImageUsage usage = ImageUsage::COLOR_ATTACHMENT | descriptor.additionalUsage;

		if (!(usage & ImageUsage::STORAGE)) {
			usage |= ImageUsage::TRANSIENT_ATTACHMENT;
		}

		return device->createImage({
			.type = descriptor.type,
			.format = attachment.format,
			.extent = descriptor.extent,
			.mipLevelCount = descriptor.mipLevelCount,
			.layerCount = descriptor.layerCount,
			.sampleCount = attachment.sampleCount,
			.usage = usage,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
			#if defined(BUILD_MODE_DEBUG)
				.name = descriptor.name,
			#endif
		});
	}

	ref<Image> RenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 subpassIndex) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateAttachmentImageDescriptor(descriptor);
		#endif

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

		return device->createImage(imageDescriptor);
	}

	ref<Image> RenderPass::createInputAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateAttachmentImageDescriptor(descriptor);
		#endif

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

		return device->createImage(imageDescriptor);
	}
}