#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLRenderPass.hpp"

namespace ProjectA {
	OpenGLRenderPass::OpenGLRenderPass(const RenderPassDescriptor& descriptor) :
		loadOperation(descriptor.loadOperation)
	{}
}