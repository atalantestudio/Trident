#include "pch.hpp"
#include "RenderCommand.hpp"
#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

namespace ProjectA {
	RendererAPI* RenderCommand::rendererApi = new OpenGLRendererAPI;
}