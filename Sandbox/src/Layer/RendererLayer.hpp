#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TriangleRenderer.hpp"

namespace ProjectA {
	class RendererLayer : public Layer {
		public:
			RendererLayer();

			virtual ~RendererLayer() = default;

			vec4<float32> getClearColor() const final override;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			//DebugPhysics2DRenderer renderer;
			TriangleRenderer renderer;
	};
}