#pragma once

#include "Engine/Camera/OrthographicCamera3D.hpp"
#include "Engine/Renderer/Subtexture.hpp"

namespace ProjectA {
	class Renderer2D {
		public:
			struct Statistics {
				uint32 drawCallCount = 0;
				uint32 quadCount = 0;

				inline uint32 getTotalVertexCount() const {
					return quadCount * 4;
				}

				inline uint32 getTotalIndexCount() const {
					return quadCount * 6;
				}
			};

		public:
			static const OrthographicCamera3D* camera;

		public:
			static void construct();

			static void destroy();

			static void beginScene();
			static void endScene();

			static void drawQuad(const vec2<float32>& position, const vec2<float32>& size, const vec4<float32>& color);
			static void drawQuad(const vec3<float32>& position, const vec2<float32>& size, const vec4<float32>& color);
			static void drawQuad(const vec2<float32>& position, const vec2<float32>& size, const ref<Texture>& texture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawQuad(const vec3<float32>& position, const vec2<float32>& size, const ref<Texture>& texture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawQuad(const vec2<float32>& position, const vec2<float32>& size, const ref<Subtexture>& subtexture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawQuad(const vec3<float32>& position, const vec2<float32>& size, const ref<Subtexture>& subtexture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});

			// NOTE: Angle is in radians.
			static void drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 angle, const vec4<float32>& color);
			static void drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 angle, const vec4<float32>& color);
			static void drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 angle, const ref<Texture>& texture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 angle, const ref<Texture>& texture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawRotatedQuad(const vec2<float32>& position, const vec2<float32>& size, float32 angle, const ref<Subtexture>& subtexture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawRotatedQuad(const vec3<float32>& position, const vec2<float32>& size, float32 angle, const ref<Subtexture>& subtexture, float32 tilingFactor = 1.0f, const vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});

			static Statistics& getStatistics();

			static void resetStatistics();

		private:
			static void startNewBatch();
	};
}