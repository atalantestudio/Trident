#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class SampleRenderer : public Renderer {
		public:
			void build() override {
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