// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "../Renderer/Physics2DRenderer.hpp"

namespace Trident {
	class Physics2DSceneLayer : public Layer {
		public:
			~Physics2DSceneLayer();

			void initialize() override {}

			void postConstruct() override;

			void preRender() override;

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override;

			void onEvent(Event& event) override;

		private:
			vec2<float32> getNdcMousePosition() const;

			void loadScene();

			void checkHover();

			bool onEvent(KeyDownEvent& event);
			bool onEvent(KeyUpEvent& event);
			bool onEvent(MouseDownEvent& event);
			bool onEvent(MouseMoveEvent& event);
			bool onEvent(MouseUpEvent& event);
			bool onEvent(MouseWheelEvent& event);
			bool onEvent(WindowResizeEvent& event);

		private:
			Scene2D* scene;
			PhysicsSimulation2D* physicsSimulation;
			Physics2DRenderer renderer;

			Input keyMap;
			bool jumping = false;

			Entity playerEntity = max<Entity>();
			Entity hoveredEntity = max<Entity>();
			Entity selectedEntity = max<Entity>();
			vec2<float32> placeholderOffset;

			vec2<float32> mousePosition = {0.0_m, 0.0_m};
			vec2<float32> mouseDelta;

			bool draggingCamera = false;
	};
}