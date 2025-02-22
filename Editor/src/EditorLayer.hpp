#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public Layer {
		private:
			OrthographicCameraController cameraController;

			ref<Framebuffer> framebuffer;

			vec4<float32> color = {0.2f, 0.3f, 0.8f, 1.0f};
			ref<Texture2D> checkerboardTexture;

			vec2<float32> viewportWindowSize;
			bool viewportWindowFocused = false;
			bool viewportWindowHovered = false;

		public:
			EditorLayer();

			virtual ~EditorLayer() = default;

			void bind() override;
			void unbind() override;

			void update(float32 deltaTime) override;

			virtual void renderImGui() override;

			void onEvent(Event& event) override;
	};
}