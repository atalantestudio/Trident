#include "pch.hpp"

#include "ProjectA/Renderer/RenderCommand.hpp"

namespace ProjectA {
	scope<RendererAPI> RenderCommand::rendererApi = RendererAPI::create();
}