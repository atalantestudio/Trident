#pragma once

#include "ProjectA.hpp"

#include "../Renderer/Physics2DRenderer.hpp"

namespace ProjectA {
	#define VISUALIZE_MODE_NONE 0
	#define VISUALIZE_MODE_CONTACT_MANIFOLD 1
	#define VISUALIZE_MODE_CLOSEST_POINTS 2
	#define VISUALIZE_MODE_REFERENCE_EDGE_TOUCHING 3

	#define VISUALIZE_MODE VISUALIZE_MODE_NONE

	class BaseLayer : public Layer {
		public:
			explicit BaseLayer();

			virtual ~BaseLayer() = default;

			void postConstruct() override;

			void update(float32 deltaTime, float32 inverseDeltaTime) override;

			void preRender() override;

			void render(float64 remainingFrameTime, ref<CommandEncoder> commandEncoder) const override;

			void onEvent(Event& event) override;

		private:
			PhysicsEngine2DDescriptor getPhysicsEngineDescriptor();

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
			PhysicsEngine2D physicsEngine;

			#ifdef BUILD_MODE_DEBUG
				VisualizationData2D visualizationData;
			#endif

			Entity hoveredEntity = max<uint32>();
			Entity selectedEntity = max<uint32>();

			vec2<float32> placeholderOffset;

			// TODO: Implement for debugging.
			// float32 selectedEntityPreviousInverseMass;
			// float32 selectedEntityPreviousInverseInertia;
	};
}