// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/NextSubpassCommand.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/API/Graphics/Descriptor/BeginRenderPassDescriptor.hpp"

namespace Trident {
	class GL::BeginRenderPassCommand final : public GL::Command {
		public:
			explicit BeginRenderPassCommand(const BeginRenderPassDescriptor& descriptor) :
				descriptor(descriptor)
			{}

			void execute() const final override {
				const ref<GL::Framebuffer> framebuffer = ref_cast<GL::Framebuffer>(descriptor.framebuffer);
				const ref<Trident::RenderPass> renderPass = framebuffer->getRenderPass();

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->framebuffer);

				sequence<GLenum> bufs(renderPass->getAttachmentCount());
				uint32 bufIndex = 0;

				while (bufIndex < bufs.count()) {
					bufs[bufIndex] = GL_COLOR_ATTACHMENT0 + bufIndex;

					bufIndex += 1;
				}

				glDrawBuffers(static_cast<GLsizei>(bufs.count()), &bufs[0]);

				uint32 attachmentIndex = 0;

				while (attachmentIndex < descriptor.clearValues.count()) {
					const Attachment& attachment = renderPass->getAttachment(attachmentIndex);

					if (attachment.loadOperation != LoadOperation::CLEAR) {
						attachmentIndex += 1;

						continue;
					}

					const ClearValue& clearValue = descriptor.clearValues[attachmentIndex];

					if (isDepthFormat(attachment.format)) {
						glClearBufferfi(GL_DEPTH_STENCIL, 0, clearValue.depth, clearValue.stencil);
					}
					else if (isSignedIntegerColorFormat(attachment.format)) {
						glClearBufferiv(GL_COLOR, attachmentIndex, clearValue.colorSigned);
					}
					else if (isUnsignedIntegerColorFormat(attachment.format)) {
						glClearBufferuiv(GL_COLOR, attachmentIndex, clearValue.colorUnsigned);
					}
					else {
						glClearBufferfv(GL_COLOR, attachmentIndex, clearValue.color);
					}

					attachmentIndex += 1;
				}

				GL::NextSubpassCommand::setDrawBuffers(framebuffer, 0);
			}

		private:
			BeginRenderPassDescriptor descriptor;
	};
}