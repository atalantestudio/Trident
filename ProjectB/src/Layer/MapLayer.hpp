// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Camera/Controller/NoClipCameraController.hpp"
#include "Engine/Layer/Layer.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	class MapLayer : public Layer {
		private:
			static constexpr float32 MOVEMENT_SPEED = 120.0f;
			static constexpr float32 MOUSE_SENSITIVITY = 0.075f;

		public:
			void initialize() override;

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override;

			void encodeOneTimeCommands() override;

			void update(float32 deltaTime, float32 inverseDeltaTime) override;

			void render(ref<CommandEncoder> commandEncoder, float64 remainingFrameTime) const override;

			void onEvent(Event& event) override;

		private:
			bool onKeyDown(KeyDownEvent& event);

			bool onMouseDown(MouseDownEvent& event);

			bool onMouseMove(MouseMoveEvent& event);

			bool onWindowResize(WindowResizeEvent& event);

			void noclipMove(float32 deltaTime);

		public:
			MapRenderer renderer;
			NoClipCameraController cameraController;
	};
}