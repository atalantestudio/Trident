#include "pch.hpp"

#include "Engine/Renderer/RenderCommand.hpp"

namespace ProjectA {
	scope<RendererAPI> RenderCommand::rendererApi = RendererAPI::create();
}