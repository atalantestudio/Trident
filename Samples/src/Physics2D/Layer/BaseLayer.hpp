// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "../Renderer/Physics2DRenderer.hpp"

namespace Trident {
	#define VISUALIZE_MODE_NONE 0
	#define VISUALIZE_MODE_CONTACT_MANIFOLD 1
	#define VISUALIZE_MODE_CLOSEST_POINTS 2
	#define VISUALIZE_MODE_REFERENCE_EDGE_TOUCHING 3

	#define VISUALIZE_MODE VISUALIZE_MODE_CONTACT_MANIFOLD

	class BaseLayer : public Layer {
		public:
			explicit BaseLayer();

			virtual ~BaseLayer() = default;

			void postConstruct() override;

			void update(float32 deltaTime, float32 inverseDeltaTime) override;

			void preRender() override;

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override;

			void onEvent(Event& event) override;

		private:
			void checkHover();

			bool onMouseDown(MouseDownEvent& event);
			bool onMouseMove(MouseMoveEvent& event);
			bool onMouseUp(MouseUpEvent& event);
			bool onMouseWheel(MouseWheelEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

		private:
			ref<Scene2DAsset> sceneAsset;
			ECS ecs;
			PhysicsSimulation2D physicsSimulation;
			Physics2DRenderer renderer;

			Entity hoveredEntity = max<Entity>();
			Entity selectedEntity = max<Entity>();

			vec2<float32> mousePosition = {0.0_m, 0.0_m};
			vec2<float32> placeholderOffset;
	};
}