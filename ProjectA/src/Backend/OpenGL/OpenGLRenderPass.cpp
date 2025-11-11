#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		uint32 totalColorAttachmentCount = 0;
		bool anyPresentedColorAttachment = false;

		// Check if a color attachment has is meant to be presented.
		for (const ref<Subpass>& subpass : subpasses) {
			for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
				if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					continue;
				}

				totalColorAttachmentCount += 1;

				DEBUG_ASSERT(colorAttachmentReference.index < attachments.count());

				const Attachment& colorAttachment = attachments[colorAttachmentReference.index];

				if (colorAttachment.finalLayout == ImageLayout::PRESENT_SRC) {
					ENGINE_ASSERT(!anyPresentedColorAttachment, "Only one color attachment can be presented.");

					presentSourceAttachmentIndex = colorAttachmentReference.index;
					anyPresentedColorAttachment = true;
				}
			}
		}

		GLint maxColorAttachmentCount;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachmentCount);

		ENGINE_ASSERT(totalColorAttachmentCount < static_cast<uint32>(maxColorAttachmentCount), "A render pass cannot contain more than [] color attachments.", maxColorAttachmentCount);
	}
}