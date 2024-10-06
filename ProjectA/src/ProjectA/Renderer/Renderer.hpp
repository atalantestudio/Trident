#pragma once

#include "ProjectA/Camera/Camera.hpp"
#include "RenderCommand.hpp"
#include "Shader.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static const Camera* camera;

		public:
			static void initialize();

			static inline RendererAPI::GraphicsAPI getGraphicsAPI() {
				return RendererAPI::getGraphicsAPI();
			}

			static void beginScene(const Camera& camera);

			static void endScene();

			static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const mat4x4<float>& transform);

			static void onWindowResize(std::uint32_t width, std::uint32_t height);
	};
}