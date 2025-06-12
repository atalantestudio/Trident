#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLShaderModule.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor)
	{
		PROFILE_SCOPE(FUNC_SIG);

		GLint maxColorAttachments;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

		DEBUG_ASSERT(colorAttachmentCount < static_cast<uint32>(maxColorAttachments));
	}

	ref<Image> OpenGLRenderPass::createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const {
		DEBUG_ASSERT(colorAttachmentIndex < colorAttachmentCount);
		DEBUG_ASSERT((resolveAttachmentCount == 0 && colorAttachmentIndex != presentAttachmentIndex) || resolveAttachmentCount > 0);

		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		const ColorAttachment& colorAttachment = colorAttachments[colorAttachmentIndex];

		ImageDescriptor colorImageDescriptor{};
		colorImageDescriptor.type = descriptor.type;
		colorImageDescriptor.format = colorAttachment.format;
		colorImageDescriptor.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::TRANSIENT_ATTACHMENT;
		colorImageDescriptor.width = descriptor.width;
		colorImageDescriptor.height = descriptor.height;
		colorImageDescriptor.depth = descriptor.depth;
		colorImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		colorImageDescriptor.layerCount = descriptor.layerCount;
		colorImageDescriptor.sampleCount = colorAttachment.sampleCount;

		return createRef<OpenGLImage>(colorImageDescriptor);
	}

	ref<Image> OpenGLRenderPass::createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const {
		DEBUG_ASSERT(depthStencil);

		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		ImageDescriptor depthStencilImageDescriptor{};
		depthStencilImageDescriptor.type = descriptor.type;
		depthStencilImageDescriptor.format = depthStencilAttachment.format;
		depthStencilImageDescriptor.usage = ImageUsage::DEPTH_STENCIL_ATTACHMENT;
		depthStencilImageDescriptor.width = descriptor.width;
		depthStencilImageDescriptor.height = descriptor.height;
		depthStencilImageDescriptor.depth = descriptor.depth;
		depthStencilImageDescriptor.mipLevelCount = descriptor.mipLevelCount;
		depthStencilImageDescriptor.layerCount = descriptor.layerCount;
		depthStencilImageDescriptor.sampleCount = depthStencilAttachment.sampleCount;

		return createRef<OpenGLImage>(depthStencilImageDescriptor);
	}
}