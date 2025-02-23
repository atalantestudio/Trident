#pragma once

#include "pch.hpp"

#include "ProjectA/Camera/Camera.hpp"
#include "ProjectA/Renderer/RendererAPI.hpp"
#include "ProjectA/Renderer/RenderPipeline.hpp"

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

			static void submit(const ref<RenderPipeline>& renderPipeline, const mat4x4<float32>& transform);

			static void onWindowResize(uint32 width, uint32 height);
	};
}