#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class SampleRenderer : public Renderer {
		public:
			void build() override {}

			void render(ref<CommandEncoder> commandEncoder) const override {}

			void resize(WindowResizeEvent& event) override {}
	};
}