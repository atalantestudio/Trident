#pragma once

#include "math/vec/vec2.hpp"
#include "math/vec/vec3.hpp"
#include "math/vec/vec4.hpp"
#include "ProjectA/Camera/OrthographicCamera.hpp"
#include "ProjectA/Renderer/Texture2D.hpp"

namespace ProjectA {
	class Renderer2D {
		public:
			static void construct();
			static void destruct();

			static void beginScene(const OrthographicCamera& camera);
			static void endScene();

			static void drawQuad(const vec2<float>& position, const vec2<float>& size, const vec4<float>& color);
			static void drawQuad(const vec3<float>& position, const vec2<float>& size, const vec4<float>& color);

			static void drawTexturedQuad(const vec2<float>& position, const vec2<float>& size, const Ref<Texture2D>& texture);
			static void drawTexturedQuad(const vec3<float>& position, const vec2<float>& size, const Ref<Texture2D>& texture);
	};
}