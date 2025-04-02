#pragma once

#include "Engine/Renderer/Texture.hpp"

// TODO: Deprecated API.
namespace ProjectA {
	class Subtexture {
		public:
			const ref<Texture>& texture;
			const vec2<float32> coordinates[4];

		public:
			static ref<Subtexture> createSprite(const ref<Texture>& texture, const vec2<float32>& position, const vec2<float32>& size, const vec2<float32>& unit = {1.0f, 1.0f});

			Subtexture(const ref<Texture>& texture, const vec2<float32>& min, const vec2<float32>& max);
	};
}