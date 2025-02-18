#pragma once

#include "pch.hpp"

#include "ProjectA/Camera/Camera.hpp"
#include "ProjectA/Renderer/RenderCommand.hpp"
#include "ProjectA/Renderer/Shader.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static const Camera* camera;

		public:
			static void initialize();

			static void destroy();

			static RendererAPI::GraphicsAPI getGraphicsAPI() {
				return RendererAPI::getGraphicsAPI();
			}

			static void beginScene(const Camera& camera);

			static void endScene();

			static void submit(const ref<Shader>& shader, const ref<VertexArray>& vertexArray, const lyah::mat4x4<float>& transform);

			static void onWindowResize(uint32 width, uint32 height);
	};
}