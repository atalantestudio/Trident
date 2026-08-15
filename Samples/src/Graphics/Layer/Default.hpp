// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "../GraphicsLayer.hpp"

namespace Trident {
	class DefaultSampleRenderer : public Renderer {
		public:
			void build() override {}

			void render(ref<CommandEncoder> commandEncoder) const override {}
	};

	DEFINE_SAMPLE_LAYER(DefaultSample);
}