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