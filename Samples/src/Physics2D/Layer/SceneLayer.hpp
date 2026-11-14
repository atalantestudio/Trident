// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "Engine/API/Physics/2D/Shape/Hull/BoxHull.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"

#include "Physics2D/Renderer/BuildOptions.hpp"
#include "Physics2D/Renderer/Physics2DRenderer.hpp"

namespace Trident {
	class Physics2DSceneLayer : public Layer {
		public:
			void initialize() override {}

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

			void LoadBoxSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			void LoadBoxPyramidSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			void LoadBoxStackingSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			void LoadCircleSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			// void LoadCircleSlidingSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			void LoadDominoSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			void LoadFrictionSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			// void LoadPolygonSample(PhysicsSimulationDescriptor2D& simulationDescriptor);
			void LoadSwingSample(PhysicsSimulationDescriptor2D& simulationDescriptor);

		private:
			ref<Scene2D> scene;
			PhysicsSimulation2D simulation;
			Physics2DRenderer renderer;
			PhysicsRendererBuildOptions2D rendererBuildOptions;

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