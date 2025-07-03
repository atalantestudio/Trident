#pragma once

#include "ProjectA.hpp"

#include "../Renderer/Physics2DRenderer.hpp"

namespace ProjectA {
	class RendererLayer : public Layer {
		public:
			void build() override;

			virtual ~RendererLayer() = default;

			void update(float64 fixedDeltaTime) override;

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

		private:
			Physics2DRenderer renderer;
			Scene2D scene;

			GJK2DResult gjkResult;

			uint32 hoveredObjectIndex = max<uint32>();
			uint32 selectedObjectIndex = max<uint32>();

			vec2<float32> placeholderOffset;
	};
}