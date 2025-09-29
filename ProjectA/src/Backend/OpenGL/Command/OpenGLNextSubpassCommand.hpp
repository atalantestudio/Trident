#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLFramebuffer.hpp"

#include "Engine/API/Rendering/Subpass.hpp"

namespace ProjectA {
	class OpenGLNextSubpassCommand final : public OpenGLCommand {
		public:
			// Selects draw buffers on the currently bound framebuffer.
			static void setDrawBuffers(const ref<Subpass>& subpass) {
				sequence<GLenum> bufs(subpass->getColorAttachmentReferenceCount());

				for (uint32 bufIndex = 0; bufIndex < bufs.count; bufIndex += 1) {
					const AttachmentReference& colorAttachmentReference = subpass->getColorAttachmentReferences()[bufIndex];

					if (colorAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						bufs[bufIndex] = GL_NONE;

						continue;
					}

					bufs[bufIndex] = GL_COLOR_ATTACHMENT0 + colorAttachmentReference.index;
				}

				// TODO(draw-buffers): Use glNamedFramebufferDrawBuffers in OpenGL 4.5+.
				glDrawBuffers(static_cast<GLsizei>(bufs.count), bufs);
			}

		public:
			explicit OpenGLNextSubpassCommand(const ref<OpenGLFramebuffer>& framebuffer, const ref<Subpass>& subpass) :
				framebuffer(framebuffer),
				subpass(subpass)
			{}

			void execute() const final override {
				setDrawBuffers(subpass);
			}

		private:
			const ref<OpenGLFramebuffer>& framebuffer;
			const ref<Subpass>& subpass;
	};
}