#pragma once

#include "pch.hpp"

#include "Engine/Camera/Camera.hpp"
#include "Engine/Renderer/GraphicsAPI.hpp"
#include "Engine/Renderer/RendererAPI.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class Renderer {
		public:
			static GraphicsAPI graphicsApi;

			static const Camera* camera;

		public:
			static void initialize();

			static void destroy();

			static GraphicsAPI getGraphicsApi();

			static scope<RendererAPI>& getRendererApi();

			static void submit(const ref<RenderPipeline>& renderPipeline, const mat4x4<float32>& transform);

			static void onWindowResize(uint32 width, uint32 height);

			// TODO: RenderPassCommandEncoder
			// void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset);

		private:
			static scope<RendererAPI> rendererApi;
	};
}