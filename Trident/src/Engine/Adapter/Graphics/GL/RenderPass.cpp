// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::RenderPass::RenderPass(const Trident::Device* device, const Trident::RenderPassDescriptor& descriptor) :
		Trident::RenderPass(device, descriptor)
	{}

	ref<Trident::RenderPass> GL::createRenderPass(const Trident::Device* device, ref<Trident::Surface> surface, const RenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::RenderPass> renderPass = createRef<GL::RenderPass>(device, descriptor);

		uint32 totalColorAttachmentCount = 0;
		bool anyPresentedColorAttachment = false;

		// Check if a color attachment has is meant to be presented.
		for (const ref<Subpass>& subpass : renderPass->subpasses) {
			for (const AttachmentReference& colorAttachmentReference : subpass->getColorAttachmentReferences()) {
				if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
					continue;
				}

				totalColorAttachmentCount += 1;

				DEBUG_ASSERT(colorAttachmentReference.index < renderPass->attachments.count());

				const Attachment& colorAttachment = renderPass->attachments[colorAttachmentReference.index];

				if (colorAttachment.finalLayout == ImageLayout::PRESENT_SRC) {
					ADAPTER_ASSERT(!anyPresentedColorAttachment, "Only one color attachment can be presented.");

					renderPass->presentSourceAttachmentIndex = colorAttachmentReference.index;

					anyPresentedColorAttachment = true;
				}
			}
		}

		GLint maxColorAttachmentCount;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachmentCount);

		ADAPTER_ASSERT(totalColorAttachmentCount < static_cast<uint32>(maxColorAttachmentCount), "A render pass cannot contain more than [] color attachments.", maxColorAttachmentCount);

		for (const Attachment& attachment : descriptor.attachments) {
			if (attachment.sampleCount > 1) {
				renderPass->multisample = true;

				break;
			}
		}

		return renderPass;
	}

	void GL::destroyRenderPass(const Trident::Device* device, const Trident::RenderPass* renderPass) {}
}