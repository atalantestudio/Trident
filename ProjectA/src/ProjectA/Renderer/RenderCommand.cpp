#include "pch.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"
#include "ProjectA/Renderer/RenderCommand.hpp"

namespace ProjectA {
	RendererAPI* RenderCommand::rendererApi = new OpenGLRendererAPI;
}