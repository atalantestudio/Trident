#pragma once

#include "ProjectA/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class RenderCommand {
		private:
			static Scope<RendererAPI> rendererApi;

		public:
			static void initialize() {
				rendererApi->initialize();
			}

			static void setViewport(std::int32_t x, std::int32_t y, std::uint32_t width, std::uint32_t height) {
				rendererApi->setViewport(x, y, width, height);
			}

			static void setClearColor(const lyah::vec4<float32>& color) {
				rendererApi->setClearColor(color);
			}

			static void clear() {
				rendererApi->clear();
			}

			static void drawIndexed(const ref<VertexArray>& vertexArray, uint32 indexCount) {
				rendererApi->drawIndexed(vertexArray, indexCount);
			}
	};
}