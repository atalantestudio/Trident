#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DefaultRenderer : public Renderer {
		public:
			DefaultRenderer() {
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