#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DefaultRenderer : public Renderer {
		public:
			DefaultRenderer() {
				VertexInputBinding vertexInputBinding(InputRate::VERTEX);
				vertexInputBinding.registerAttribute<mat3x3<float32>>(); // world transform
				vertexInputBinding.registerAttribute<mat3x3<float32>>(); // texture transform
				vertexInputBinding.registerAttribute<uint32>(); // font index
				vertexInputBinding.registerAttribute<float32>(); // outline width
				vertexInputBinding.registerAttribute<vec4<float32>>(); // color
				vertexInputBinding.registerAttribute<vec4<float32>>(); // outline color

				BREAKPOINT();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override {
				BREAKPOINT();
			}

			void resize(WindowResizeEvent& event) override {
				BREAKPOINT();
			}
	};
}