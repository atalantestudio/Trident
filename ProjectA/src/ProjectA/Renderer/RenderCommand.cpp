#include "pch.hpp"

#include "ProjectA/Renderer/RenderCommand.hpp"

namespace ProjectA {
	Scope<RendererAPI> RenderCommand::rendererApi = RendererAPI::create();
}