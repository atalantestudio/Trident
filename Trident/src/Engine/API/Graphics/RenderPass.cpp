// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"

namespace Trident {
	#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
		static void validateAttachmentImageDescriptor(const AttachmentImageDescriptor& descriptor) {
			Validator validator;

			if (descriptor.type >= ImageType::count) {
				VALIDATION_ERROR("descriptor.type must be a valid ImageType value.");
			}

			if (descriptor.extent.x == 0) {
				VALIDATION_ERROR("descriptor.extent.x must be greater than 0.");
			}

			if (descriptor.extent.y == 0) {
				VALIDATION_ERROR("descriptor.extent.y must be greater than 0.");
			}

			if (descriptor.extent.z == 0) {
				VALIDATION_ERROR("descriptor.extent.z must be greater than 0.");
			}

			if (descriptor.mipLevelCount == 0) {
				VALIDATION_ERROR("descriptor.mipLevelCount must be greater than 0.");
			}

			if (descriptor.layerCount == 0) {
				VALIDATION_ERROR("descriptor.layerCount must be greater than 0.");
			}
		}
	#endif

	RenderPass::RenderPass(const Device* device, const RenderPassDescriptor& descriptor) :
		device(device),
		attachments(descriptor.attachments),
		subpasses(descriptor.subpasses),
		totalAttachmentCount(static_cast<uint32>(descriptor.attachments.count()))
	{
		for (const Attachment& attachment : attachments) {
			if (attachment.sampleCount != ImageSampleCount::x1 && attachment.finalLayout == ImageLayout::PRESENT_SRC) {
				totalAttachmentCount += 1;

				break;
			}
		}

		for (Subpass& subpass : subpasses) {
			ImageSampleCount rasterizationSampleCount = ImageSampleCount::x1;

			for (const AttachmentReference& colorAttachmentReference : subpass.colorAttachmentReferences) {
				if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					continue;
				}

				const Attachment& attachment = attachments[colorAttachmentReference.index];

				if (attachment.sampleCount != ImageSampleCount::x1) {
					attachmentReferenceCount += 1;
				}

				if (rasterizationSampleCount == ImageSampleCount::x1) {
					rasterizationSampleCount = attachment.sampleCount;

					continue;
				}

				// TODO(graphics/render-pass/attachment-sample-count): There is no need for each attachment to store a sample count.
				ENGINE_ASSERT(rasterizationSampleCount == attachment.sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");
			}

			if (subpass.depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
				if (rasterizationSampleCount == ImageSampleCount::x1) {
					rasterizationSampleCount = attachments[subpass.depthStencilAttachmentReference.index].sampleCount;
				}
				else {
					// TODO(graphics/render-pass/attachment-sample-count): There is no need for each attachment to store a sample count.
					ENGINE_ASSERT(rasterizationSampleCount == attachments[subpass.depthStencilAttachmentReference.index].sampleCount, "All color and depth/stencil attachments in a subpass must have the same sample count.");
				}

				attachmentReferenceCount += 1;
			}

			subpass.rasterizationSampleCount = rasterizationSampleCount;

			attachmentReferenceCount += static_cast<uint32>(subpass.colorAttachmentReferences.count() + subpass.inputAttachmentReferences.count());
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

	uint32 RenderPass::getTotalAttachmentCount() const {
		return totalAttachmentCount;
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

	const sequence<Subpass>& RenderPass::getSubpasses() const {
		return subpasses;
	}

	const Subpass& RenderPass::getSubpass(uint32 subpassIndex) const {
		DEBUG_ASSERT(subpassIndex < subpasses.count());

		return subpasses[subpassIndex];
	}

	ref<Image> RenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateAttachmentImageDescriptor(descriptor);
		#endif

		DEBUG_ASSERT(attachmentIndex < attachments.count());

		const Attachment& attachment = attachments[attachmentIndex];

		DEBUG_ASSERT(attachment.sampleCount != ImageSampleCount::x1 || (attachment.sampleCount == ImageSampleCount::x1 && attachment.finalLayout != ImageLayout::PRESENT_SRC));

		return device->createDynamicImage({
			.type = descriptor.type,
			.format = attachment.format,
			.extent = descriptor.extent,
			.mipLevelCount = descriptor.mipLevelCount,
			.layerCount = descriptor.layerCount,
			.sampleCount = attachment.sampleCount,
			.usage = ImageUsage::COLOR_ATTACHMENT | descriptor.additionalUsage,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
			#if defined(DEBUG_OBJECT_NAME)
				.name = descriptor.name,
			#endif
		});
	}

	ref<Image> RenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 subpassIndex) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateAttachmentImageDescriptor(descriptor);
		#endif

		DEBUG_ASSERT(subpassIndex < subpasses.count());

		const Subpass& subpass = subpasses[subpassIndex];

		ENGINE_ASSERT(subpass.depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX, "The subpass doesn't use a depth/stencil attachment.");

		const uint32 attachmentIndex = subpass.depthStencilAttachmentReference.index;

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
			const Subpass& subpass = subpasses[nextSubpassIndex];

			for (const AttachmentReference& inputAttachmentReference : subpass.inputAttachmentReferences) {
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

		return device->createDynamicImage(imageDescriptor);
	}

	ref<Image> RenderPass::createInputAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 attachmentIndex) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
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

		return device->createDynamicImage(imageDescriptor);
	}
}