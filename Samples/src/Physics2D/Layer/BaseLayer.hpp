#pragma once

#include "ProjectA.hpp"

#include "../Renderer/Physics2DRenderer.hpp"

namespace ProjectA {
	class BaseLayer : public Layer {
		public:
			explicit BaseLayer();

			virtual ~BaseLayer() = default;

			void update(float32 deltaTime) override;

			void preRender() override;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const override;

			void onEvent(Event& event) override;

		private:
			void checkHover();

			bool onMouseDown(MouseDownEvent& event);
			bool onMouseMove(MouseMoveEvent& event);
			bool onMouseUp(MouseUpEvent& event);
			bool onMouseWheel(MouseWheelEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

		protected:
			Physics2DRenderer renderer;
			Scene2D scene;

		private:
			PhysicsEngine physicsEngine;
			VisualizationData2D visualizationData;

			Entity hoveredEntity = max<uint32>();
			Entity selectedEntity = max<uint32>();

			vec2<float32> placeholderOffset;

			// float32 selectedEntityPreviousInverseMass;
			// float32 selectedEntityPreviousInverseInertia;
	};
}