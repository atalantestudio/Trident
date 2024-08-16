#pragma once

#include "ProjectA/Renderer/RendererAPI.hpp"

namespace ProjectA {
	class OpenGLRendererAPI : public RendererAPI {
		public:
			virtual void initialize() override;

			virtual void setClearColor(const vec4<float>& color) const override;

			virtual void clear() const override;

			virtual void drawIndexed(const Ref<VertexArray>& vertexArray) const override;
	};
}