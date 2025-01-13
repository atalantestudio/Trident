#pragma once

#include "ProjectA/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			virtual void initialize() override;

			virtual void setViewport(std::int32_t x, std::int32_t, std::uint32_t width, std::uint32_t height) const override;

			virtual void setClearColor(const vec4<float>& color) const override;

			virtual void clear() const override;

			virtual void drawIndexed(const ref<VertexArray>& vertexArray) const override;
	};
}