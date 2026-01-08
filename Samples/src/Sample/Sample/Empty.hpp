#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class SampleRenderer : public Renderer {
		public:
			void build() override {}

			void render(ref<CommandEncoder> commandEncoder) const override {}

			void resize(WindowResizeEvent& event) override {}
	};
}