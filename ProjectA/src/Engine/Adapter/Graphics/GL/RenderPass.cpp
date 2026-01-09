// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	GL::RenderPass::RenderPass(const RenderPassDescriptor& descriptor) :
		ProjectA::RenderPass(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		uint32 totalColorAttachmentCount = 0;
		bool anyPresentedColorAttachment = false;

		// Check if a color attachment has is meant to be presented.
		for (const ref<Subpass> subpass : subpasses) {
			for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
				if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					continue;
				}

				totalColorAttachmentCount += 1;

				DEBUG_ASSERT(colorAttachmentReference.index < attachments.count());

				const Attachment& colorAttachment = attachments[colorAttachmentReference.index];

				if (colorAttachment.finalLayout == ImageLayout::PRESENT_SRC) {
					ADAPTER_ASSERT(!anyPresentedColorAttachment, "Only one color attachment can be presented.");

					presentSourceAttachmentIndex = colorAttachmentReference.index;
					anyPresentedColorAttachment = true;
				}
			}
		}

		GLint maxColorAttachmentCount;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachmentCount);

		ADAPTER_ASSERT(totalColorAttachmentCount < static_cast<uint32>(maxColorAttachmentCount), "A render pass cannot contain more than [] color attachments.", maxColorAttachmentCount);
	}
}