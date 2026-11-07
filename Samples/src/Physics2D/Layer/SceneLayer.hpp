// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "Physics2D/Renderer/Renderer.hpp"

namespace Trident {
	class Physics2DSceneLayer : public Layer {
		public:
			void initialize() override;

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override;

			void postConstruct() override;

			void preRender() override;

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override;

			void onEvent(Event& event) override;

		private:
			vec2<float32> getNdcMousePosition() const;

			void checkHover();

			void registerHullForRender(const HullHeader2D* hullHeader);

			bool onEvent(KeyDownEvent& event);
			bool onEvent(KeyUpEvent& event);
			bool onEvent(MouseDownEvent& event);
			bool onEvent(MouseMoveEvent& event);
			bool onEvent(MouseUpEvent& event);
			bool onEvent(MouseWheelEvent& event);
			bool onEvent(WindowResizeEvent& event);

		private:
			PhysicsSimulation2D simulation;
			Physics2DRenderer renderer;
			PhysicsRendererBuildOptions2D rendererOptions;

			Input keyMap;
			bool jumping = false;

			Entity hoveredEntity = max<Entity>();
			Entity selectedEntity = max<Entity>();
			vec2<float32> placeholderOffset;

			vec2<float32> mousePosition = {0.0_m, 0.0_m};
			vec2<float32> mouseDelta;

			BoxHull2D boxHull;
			uint32 hullIndex = 0;

			bool draggingCamera = false;
	};
}