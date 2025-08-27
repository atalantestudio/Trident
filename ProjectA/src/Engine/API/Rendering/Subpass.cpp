#include "pch.hpp"

#include "Engine/API/Rendering/Subpass.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	ref<Image> Subpass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const {
		DEBUG_ASSERT(colorAttachmentIndex < colorAttachments.count);
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

		DEBUG_ASSERT(resolveAttachmentCount > 0 || (resolveAttachmentCount == 0 && colorAttachment.finalImageLayout != ImageLayout::PRESENT_SRC));

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = colorAttachment.format;
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = colorAttachment.sampleCount;
		imageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT;
		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		return Renderer::createImage(imageDescriptor);
	}

	ref<Image> Subpass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const {
		DEBUG_ASSERT(depthStencil);
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		ImageDescriptor imageDescriptor{};
		imageDescriptor.type = descriptor.type;
		imageDescriptor.format = static_cast<ImageFormat>(depthStencilAttachment.format);
		imageDescriptor.extent = descriptor.extent;
		imageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		imageDescriptor.layerCount = descriptor.layerCount;
		imageDescriptor.sampleCount = depthStencilAttachment.sampleCount;
		imageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT;
		imageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		return Renderer::createImage(imageDescriptor);
	}
}