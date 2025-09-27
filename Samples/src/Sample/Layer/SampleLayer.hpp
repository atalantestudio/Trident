#pragma once

#include "Engine/Layer/Layer.hpp"

#define RENDERER_ARRAY_TEXTURE 0
#define RENDERER_COLOR_BLEND 1
#define RENDERER_COMPUTE_SHADER 2
#define RENDERER_COMPUTE_SHADER_INDIRECT 3
#define RENDERER_DEFAULT 4
#define RENDERER_DEPTH_TEST 5
#define RENDERER_IMAGE 6
#define RENDERER_IMAGE_ASSET 7
#define RENDERER_INPUT_ATTACHMENT 8
#define RENDERER_MULTISAMPLE 9
#define RENDERER_TEXT 10
#define RENDERER_TEXTURE_ARRAY 11
#define RENDERER_TRIANGLE 12
#define RENDERER_TRIANGLE_INDEXED 13
#define RENDERER_TRIANGLE_INDEXED_INDIRECT 14
#define RENDERER_TRIANGLE_INDIRECT 15

// Select the renderer to run.
#define RENDERER RENDERER_TRIANGLE_INDIRECT

#if RENDERER == RENDERER_ARRAY_TEXTURE
	#include "../Renderer/ArrayTextureRenderer.hpp"

	#define RENDERER_CLASS ArrayTextureRenderer
#endif

#if RENDERER == RENDERER_COLOR_BLEND
	#include "../Renderer/ColorBlendRenderer.hpp"

	#define RENDERER_CLASS ColorBlendRenderer
#endif

#if RENDERER == RENDERER_COMPUTE_SHADER
	#include "../Renderer/ComputeShaderRenderer.hpp"

	#define RENDERER_CLASS ComputeShaderRenderer
#endif

#if RENDERER == RENDERER_COMPUTE_SHADER_INDIRECT
	#include "../Renderer/ComputeShaderIndirectRenderer.hpp"

	#define RENDERER_CLASS ComputeShaderIndirectRenderer
#endif

#if RENDERER == RENDERER_DEFAULT
	#include "../Renderer/DefaultRenderer.hpp"

	#define RENDERER_CLASS DefaultRenderer
#endif

#if RENDERER == RENDERER_DEPTH_TEST
	#include "../Renderer/DepthTestRenderer.hpp"

	#define RENDERER_CLASS DepthTestRenderer
#endif

#if RENDERER == RENDERER_IMAGE
	#include "../Renderer/ImageRenderer.hpp"

	#define RENDERER_CLASS ImageRenderer
#endif

#if RENDERER == RENDERER_IMAGE_ASSET
	#include "../Renderer/ImageAssetRenderer.hpp"

	#define RENDERER_CLASS ImageAssetRenderer
#endif

#if RENDERER == RENDERER_INPUT_ATTACHMENT
	#include "../Renderer/InputAttachmentRenderer.hpp"

	#define RENDERER_CLASS InputAttachmentRenderer
#endif

#if RENDERER == RENDERER_MULTISAMPLE
	#include "../Renderer/MultisampleRenderer.hpp"

	#define RENDERER_CLASS MultisampleRenderer
#endif

#if RENDERER == RENDERER_TEXT
	#include "../Renderer/TextRenderer.hpp"

	#define RENDERER_CLASS TextRenderer
#endif

#if RENDERER == RENDERER_TEXTURE_ARRAY
	#include "../Renderer/TextureArrayRenderer.hpp"

	#define RENDERER_CLASS TextureArrayRenderer
#endif

#if RENDERER == RENDERER_TRIANGLE
	#include "../Renderer/TriangleRenderer.hpp"

	#define RENDERER_CLASS TriangleRenderer
#endif

#if RENDERER == RENDERER_TRIANGLE_INDEXED
	#include "../Renderer/TriangleIndexedRenderer.hpp"

	#define RENDERER_CLASS TriangleIndexedRenderer
#endif

#if RENDERER == RENDERER_TRIANGLE_INDIRECT
	#include "../Renderer/TriangleIndirectRenderer.hpp"

	#define RENDERER_CLASS TriangleIndirectRenderer
#endif

#if RENDERER == RENDERER_TRIANGLE_INDEXED_INDIRECT
	#include "../Renderer/TriangleIndexedIndirectRenderer.hpp"

	#define RENDERER_CLASS TriangleIndexedIndirectRenderer
#endif

namespace ProjectA {
	class SampleLayer : public Layer {
		public:
			void build() override {
				renderer.setLayer(this);
				renderer.build();
			}

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