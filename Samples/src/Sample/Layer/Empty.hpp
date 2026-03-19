// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../SampleLayer.hpp"

namespace Trident {
	class SampleEmptyRenderer : public Renderer {
		public:
			void build() override {}

			void render(ref<CommandEncoder> commandEncoder) const override {}

			void resize(WindowResizeEvent& event) override {}
	};

	DEFINE_SAMPLE_LAYER(SampleEmpty);
}