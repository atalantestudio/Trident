// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Layer/Layer.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	class MapLayer : public Layer {
		public:
			void initialize() final override {
				renderer.setLayer(this);
				renderer.build();
				renderer.postBuild();
			}

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const final override {
				renderer.render(commandEncoder);
			}

		public:
			MapRenderer renderer;
	};
}