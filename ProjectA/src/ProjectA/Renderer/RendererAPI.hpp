#pragma once

#include "ProjectA/Renderer/VertexArray.hpp"

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
			static Scope<RendererAPI> create();

		public:
			static inline GraphicsAPI getGraphicsAPI() {
				return graphicsApi;
			}

			virtual void initialize() = 0;

			virtual void setViewport(std::int32_t x, std::int32_t, std::uint32_t width, std::uint32_t height) const = 0;

			virtual void setClearColor(const vec4<float>& color) const = 0;

			virtual void clear() const = 0;

			virtual void drawIndexed(const ref<VertexArray>& vertexArray) const = 0;
	};
}