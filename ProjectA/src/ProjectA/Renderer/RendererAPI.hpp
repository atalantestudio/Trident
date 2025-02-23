#pragma once

#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	class RendererAPI {
		public:
			enum class GraphicsAPI {
				None = 0,
				OpenGL = 1,
				Vulkan = 2,
			};

		private:
			static GraphicsAPI graphicsApi;

		public:
			static scope<RendererAPI> create();

			static GraphicsAPI getGraphicsAPI() {
				return graphicsApi;
			}

			virtual ~RendererAPI() = default;

			virtual void initialize() = 0;

			virtual void setViewport(std::int32_t x, std::int32_t, std::uint32_t width, std::uint32_t height) const = 0;

			virtual void setClearColor(const lyah::vec4<float>& color) const = 0;

			virtual void clear() const = 0;

			virtual void drawIndexed(const ref<VertexArray>& vertexArray, uint32 indexCount) const = 0;
	};
}