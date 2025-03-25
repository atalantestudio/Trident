#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLFramebuffer.hpp"

namespace ProjectA {
	static const uint32 maxFramebufferSize = 8192;

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferDescriptor& descriptor) {
		reset();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer() {
		glDeleteFramebuffers(1, &id);
	}

	void OpenGLFramebuffer::reset() {
		if (id > 0) {
			glDeleteFramebuffers(1, &id);
			glDeleteTextures(1, &colorAttachmentId);
			glDeleteTextures(1, &depthAttachmentId);
		}

		const GLsizei width = static_cast<GLsizei>(descriptor.size[0]);
		const GLsizei height = static_cast<GLsizei>(descriptor.size[1]);

		glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachmentId);
		glBindTexture(GL_TEXTURE_2D, colorAttachmentId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachmentId);
		glBindTexture(GL_TEXTURE_2D, depthAttachmentId);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);

		glCreateFramebuffers(1, &id);

		bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachmentId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentId, 0);

		const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

		ENGINE_ASSERT(complete, "Incomplete framebuffer.");

		unbind();
	}

	uint32 OpenGLFramebuffer::getColorAttachmentId() const {
		return colorAttachmentId;
	}

	uint32 OpenGLFramebuffer::getDepthAttachmentId() const {
		return depthAttachmentId;
	}

	void OpenGLFramebuffer::bind() {
		const GLsizei width = static_cast<GLsizei>(descriptor.size[0]);
		const GLsizei height = static_cast<GLsizei>(descriptor.size[1]);

		glBindFramebuffer(GL_FRAMEBUFFER, id);
		glViewport(0, 0, width, height);
	}

	void OpenGLFramebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::resize(const vec2<float32>& size) {
		if (size[0] < 0.0f || size[1] < 0.0f) {
			ENGINE_WARN("Framebuffer resize failed with size = []", size);

			return;
		}

		descriptor.size = size;

		reset();
	}
}