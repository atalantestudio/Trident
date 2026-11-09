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

			void LoadBoxSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadBoxPyramidSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadBoxStackingSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadCircleSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadCircleSlidingSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadDominoSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadFrictionSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadPolygonSample(PhysicsSimulation2DDescriptor& simulationDescriptor);
			void LoadSwingSample(PhysicsSimulation2DDescriptor& simulationDescriptor);

		private:
			PhysicsSimulation2D simulation;
			Physics2DRenderer renderer;
			PhysicsRendererBuildOptions2D rendererOptions;

			sequence<BoxHull2D> boxHulls;

			Input keyMap;
			bool jumping = false;

			Entity hoveredEntity = max<Entity>();
			Entity selectedEntity = max<Entity>();
			vec2<float32> placeholderOffset;

			vec2<float32> mousePosition = {0.0_m, 0.0_m};
			vec2<float32> mouseDelta;

			bool draggingCamera = false;
	};
}