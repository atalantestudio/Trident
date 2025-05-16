#include "pch.hpp"

#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		loadOperation(descriptor.loadOperation)
	{
		// TODO: MSAA color attachment.
	}
}