#pragma once

#include "ProjectA.hpp"

#include "../Renderer/DebugPhysics2DRenderer.hpp"

namespace ProjectA {
	class RendererLayer : public Layer {
		public:
			RendererLayer();

			virtual ~RendererLayer() = default;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override;

			void onEvent(Event& event) final override;

		private:
			bool onMouseWheel(MouseWheelEvent& event) const;

		private:
			DebugPhysics2DRenderer renderer;
	};
}