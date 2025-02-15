#pragma once

#include "ProjectA/Camera/OrthographicCamera.hpp"
#include "ProjectA/Renderer/Texture2D.hpp"

namespace ProjectA {
	class Renderer2D {
		public:
			static void construct();

			static void destruct();

			static void beginScene(const OrthographicCamera& camera);
			static void endScene();

			static void drawQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, const lyah::vec4<float32>& color);
			static void drawQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, const lyah::vec4<float32>& color);
			static void drawQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, const ref<Texture2D>& texture, float32 tilingFactor = 1.0f, const lyah::vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, const ref<Texture2D>& texture, float32 tilingFactor = 1.0f, const lyah::vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});

			static void drawRotatedQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, float32 rotation, const lyah::vec4<float32>& color);
			static void drawRotatedQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, float32 rotation, const lyah::vec4<float32>& color);
			static void drawRotatedQuad(const lyah::vec2<float32>& position, const lyah::vec2<float32>& size, float32 rotation, const ref<Texture2D>& texture, float32 tilingFactor = 1.0f, const lyah::vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
			static void drawRotatedQuad(const lyah::vec3<float32>& position, const lyah::vec2<float32>& size, float32 rotation, const ref<Texture2D>& texture, float32 tilingFactor = 1.0f, const lyah::vec4<float32>& tintColor = {1.0f, 1.0f, 1.0f, 1.0f});
	};
}