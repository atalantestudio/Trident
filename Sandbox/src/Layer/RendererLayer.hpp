#pragma once

#include "ProjectA.hpp"

#include "../Renderer/DebugPhysics2DRenderer.hpp"

namespace ProjectA {
	class RendererLayer : public Layer {
		public:
			RendererLayer();

			virtual ~RendererLayer() = default;

			void update(float64 fixedDeltaTime) final override;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override;

			void onEvent(Event& event) final override;

		private:
			void advanceSimulation(Object2D& A, float32 t) const;

			bool onMouseMove(MouseMoveEvent& event);

			bool onMouseWheel(MouseWheelEvent& event);

			bool onWindowResize(WindowResizeEvent& event) const;

		private:
			DebugPhysics2DRenderer renderer;
			Scene2D scene;

			uint32 hoveredObjectIndex = std::numeric_limits<uint32>::max();
	};
}