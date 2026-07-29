// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/API/Graphics/Subpass.hpp"

namespace Trident {
	class GL::NextSubpassCommand final : public GL::Command {
		public:
			// Selects draw buffers on the currently bound framebuffer.
			static void setDrawBuffers(ref<GL::Framebuffer> framebuffer, uint32 subpassIndex) {
				const Subpass& subpass = framebuffer->getRenderPass()->getSubpass(subpassIndex);

				sequence<GLenum> bufs(subpass.colorAttachmentReferences.count());
				uint32 bufIndex = 0;

				while (bufIndex < bufs.count()) {
					const AttachmentReference& colorAttachmentReference = subpass.colorAttachmentReferences[bufIndex];

					if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						bufs[bufIndex] = GL_NONE;
						bufIndex += 1;

						continue;
					}

					bufs[bufIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentReference.index;
					bufIndex += 1;
				}

				glDrawBuffers(static_cast<GLsizei>(bufs.count()), &bufs[0]);
			}

		public:
			explicit NextSubpassCommand(ref<GL::Framebuffer> framebuffer, uint32 subpassIndex) :
				framebuffer(framebuffer),
				subpassIndex(subpassIndex)
			{}

			void execute() const final override {
				setDrawBuffers(framebuffer, subpassIndex);
			}

		private:
			ref<GL::Framebuffer> framebuffer;
			uint32 subpassIndex;
	};
}