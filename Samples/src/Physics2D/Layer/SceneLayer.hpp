// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "../Renderer/Physics2DRenderer.hpp"

namespace Trident {
	class Physics2DSceneLayer : public Layer {
		public:
			explicit Physics2DSceneLayer();

			virtual ~Physics2DSceneLayer() = default;

			PhysicsEngine2DDescriptor getPhysicsEngineDescriptor();

			void build() override {}

			void postConstruct() override;

			void update(float32 deltaTime, float32 inverseDeltaTime) override;

			void preRender() override;

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override;

			void onEvent(Event& event) override;

		private:
			void checkHover();

			bool onEvent(KeyDownEvent& event);
			bool onEvent(KeyUpEvent& event);
			bool onEvent(MouseDownEvent& event);
			bool onEvent(MouseMoveEvent& event);
			bool onEvent(MouseUpEvent& event);
			bool onEvent(MouseWheelEvent& event);
			bool onEvent(WindowResizeEvent& event);

		private:
			PhysicsEngine2D physicsEngine;
			Physics2DRenderer renderer;
			Input keyMap;
			bool jumping = false;

			Entity playerEntity = max<uint32>();
			Entity hoveredEntity = max<uint32>();
			Entity selectedEntity = max<uint32>();
			vec2<float32> placeholderOffset;

			#if defined(BUILD_MODE_DEBUG)
				VisualizationData2D visualizationData{};
			#endif
	};
}