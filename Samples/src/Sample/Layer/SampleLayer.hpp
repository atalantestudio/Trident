#pragma once

#include "Engine/Layer/Layer.hpp"

#define RENDERER_COLOR_BLEND 0
#define RENDERER_COMPUTE_SHADER 1
#define RENDERER_DEPTH_TEST 2
#define RENDERER_DEFAULT 3
#define RENDERER_MULTISAMPLE 4
#define RENDERER_TEXT 5
#define RENDERER_TEXTURE 6
#define RENDERER_TEXTURE_ARRAY 7
#define RENDERER_TEXTURES 8
#define RENDERER_TRIANGLE 9

// Select the renderer to view.
#define RENDERER RENDERER_TEXTURES

#if RENDERER == RENDERER_COLOR_BLEND
	#include "../Renderer/ColorBlendRenderer.hpp"

	#define RENDERER_CLASS ColorBlendRenderer
#endif

#if RENDERER == RENDERER_COMPUTE_SHADER
	#include "../Renderer/ComputeShaderRenderer.hpp"

	#define RENDERER_CLASS ComputeShaderRenderer
#endif

#if RENDERER == RENDERER_DEFAULT
	#include "../Renderer/DefaultRenderer.hpp"

	#define RENDERER_CLASS DefaultRenderer
#endif

#if RENDERER == RENDERER_DEPTH_TEST
	#include "../Renderer/DepthTestRenderer.hpp"

	#define RENDERER_CLASS DepthTestRenderer
#endif

#if RENDERER == RENDERER_MULTISAMPLE
	#include "../Renderer/MultisampleRenderer.hpp"

	#define RENDERER_CLASS MultisampleRenderer
#endif

#if RENDERER == RENDERER_TEXT
	#include "../Renderer/TextRenderer.hpp"

	#define RENDERER_CLASS TextRenderer
#endif

#if RENDERER == RENDERER_TEXTURE
	#include "../Renderer/TextureRenderer.hpp"

	#define RENDERER_CLASS TextureRenderer
#endif

#if RENDERER == RENDERER_TEXTURE_ARRAY
	#include "../Renderer/TextureArrayRenderer.hpp"

	#define RENDERER_CLASS TextureArrayRenderer
#endif

#if RENDERER == RENDERER_TEXTURES
	#include "../Renderer/TexturesRenderer.hpp"

	#define RENDERER_CLASS TexturesRenderer
#endif

#if RENDERER == RENDERER_TRIANGLE
	#include "../Renderer/TriangleRenderer.hpp"

	#define RENDERER_CLASS TriangleRenderer
#endif

namespace ProjectA {
	class SampleLayer : public Layer {
		public:
			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);

				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
			}

		private:
			bool onWindowResize(WindowResizeEvent& event) {
				renderer.resize(event);

				return true;
			}

		private:
			RENDERER_CLASS renderer;
	};
}