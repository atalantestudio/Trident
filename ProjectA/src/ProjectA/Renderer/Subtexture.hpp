#pragma once

#include "ProjectA/Renderer/Texture2D.hpp"

namespace ProjectA {
	class Subtexture {
		public:
			const ref<Texture2D> texture;
			const vec2<float32> coordinates[4];

		public:
			static ref<Subtexture> createSprite(const ref<Texture2D>& texture, const vec2<float32>& position, const vec2<float32>& size, const vec2<float32>& unit = {1.0f, 1.0f});

			Subtexture(const ref<Texture2D>& texture, const vec2<float32>& min, const vec2<float32>& max);
	};
}