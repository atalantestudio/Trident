// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/API/Rendering/Subpass.hpp"

namespace Trident {
	class GL::NextSubpassCommand final : public GL::Command {
		public:
			// Selects draw buffers on the currently bound framebuffer.
			static void setDrawBuffers(ref<GL::Framebuffer> framebuffer, ref<Subpass> subpass) {
				const view<AttachmentReference> colorAttachmentReferences = subpass->getColorAttachmentReferences();

				sequence<GLenum> bufs(colorAttachmentReferences.count());
				uint32 bufIndex = 0;

				while (bufIndex < bufs.count()) {
					const AttachmentReference& colorAttachmentReference = colorAttachmentReferences[bufIndex];

					if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						bufs[bufIndex] = GL_NONE;
						bufIndex += 1;

						continue;
					}

					bufs[bufIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentReference.index;
					bufIndex += 1;
				}

				#if OPENGL_VERSION >= OPENGL_VERSION_4_5
					glNamedFramebufferDrawBuffers(framebuffer->framebuffer, static_cast<GLsizei>(bufs.count()), &bufs[0]);
				#else
					glDrawBuffers(static_cast<GLsizei>(bufs.count()), &bufs[0]);
				#endif
			}

		public:
			explicit NextSubpassCommand(ref<GL::Framebuffer> framebuffer, ref<Subpass> subpass) :
				framebuffer(framebuffer),
				subpass(subpass)
			{}

			void execute() const final override {
				setDrawBuffers(framebuffer, subpass);
			}

		private:
			ref<GL::Framebuffer> framebuffer;
			ref<Subpass> subpass;
	};
}