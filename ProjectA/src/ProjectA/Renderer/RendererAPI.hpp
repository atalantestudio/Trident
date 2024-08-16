#pragma once

#include "math/vec/vec4.hpp"
#include "VertexArray.hpp"

namespace ProjectA {
	class RendererAPI {
		public:
			enum class GraphicsAPI {
				None = 0,
				OpenGL = 1
			};

		private:
			static GraphicsAPI graphicsApi;

		public:
			static inline GraphicsAPI getGraphicsAPI() {
				return graphicsApi;
			}

			virtual void setClearColor(const vec4<float>& color) const = 0;

			virtual void clear() const = 0;

			virtual void drawIndexed(const Ref<VertexArray>& vertexArray) const = 0;
	};
}