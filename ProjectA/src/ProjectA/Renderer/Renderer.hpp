#pragma once

#include "ProjectA/Camera/Camera.hpp"
#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static Camera* camera;

		public:
			static inline RendererAPI::GraphicsAPI getGraphicsAPI() {
				return RendererAPI::getGraphicsAPI();
			}

			static void beginScene(Camera& camera);

			static void endScene();

			static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const mat4x4<float>& transform);
	};
}