#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor)
	{
		PROFILE_SCOPE(FUNC_SIG);

		uint32 totalColorAttachmentCount = 0;

		for (uint32 subpassIndex = 0; subpassIndex < subpasses.count; subpassIndex += 1) {
			const ref<Subpass>& subpass = subpasses[subpassIndex];

			totalColorAttachmentCount += subpass->getColorAttachmentCount() + subpass->getResolveAttachmentCount() + static_cast<uint32>(subpass->hasDepthStencilAttachment());

			for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < subpass->getColorAttachmentCount(); colorAttachmentIndex += 1) {
				const ColorAttachment& colorAttachment = subpass->getColorAttachments()[colorAttachmentIndex];

				if (colorAttachment.finalImageLayout == ImageLayout::PRESENT_SRC) {
					presentSourceAttachmentIndex = colorAttachmentIndex;

					break;
				}
			}
		}

		GLint maxColorAttachmentCount;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachmentCount);

		ENGINE_ASSERT(totalColorAttachmentCount < static_cast<uint32>(maxColorAttachmentCount), "A render pass cannot contain more than [] color attachments.", maxColorAttachmentCount);
	}
}