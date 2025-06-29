#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		RenderPass(descriptor)
	{
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: Check per subpass instead?
		/* GLint maxColorAttachments;

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

		DEBUG_ASSERT(colorAttachmentCount < static_cast<uint32>(maxColorAttachments)); */
	}
}