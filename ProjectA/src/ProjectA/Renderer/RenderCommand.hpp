#pragma once

#include "RendererAPI.hpp"

namespace ProjectA {
	class RenderCommand {
		private:
			static RendererAPI* rendererApi;

		public:
			static inline void initialize() {
				rendererApi->initialize();
			}

			static inline void setViewport(std::int32_t x, std::int32_t y, std::uint32_t width, std::uint32_t height) {
				rendererApi->setViewport(x, y, width, height);
			}

			static inline void setClearColor(const vec4<float>& color) {
				rendererApi->setClearColor(color);
			}

			static inline void clear() {
				rendererApi->clear();
			}

			static inline void drawIndexed(const Ref<VertexArray>& vertexArray) {
				rendererApi->drawIndexed(vertexArray);
			}
	};
}