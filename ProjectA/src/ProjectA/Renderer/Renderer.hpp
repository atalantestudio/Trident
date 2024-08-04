#pragma once

#include "RenderCommand.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static inline RendererAPI::GraphicsAPI getGraphicsAPI() {
				return RendererAPI::getGraphicsAPI();
			}

			static void beginScene();

			static void endScene();

			static void submit(const std::shared_ptr<VertexArray>& vertexArray);
	};
}