#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public Layer {
		private:
			OrthographicCamera3DController cameraController;

			ref<Framebuffer> framebuffer;

			vec4<float32> color = {0.2f, 0.3f, 0.8f, 1.0f};
			ref<Texture> checkerboardTexture;

			vec2<float32> viewportWindowSize;
			bool viewportWindowFocused = false;
			bool viewportWindowHovered = false;

		public:
			EditorLayer();

			virtual ~EditorLayer() = default;

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void renderImGui() override;

			void onEvent(Event& event) override;
	};
}