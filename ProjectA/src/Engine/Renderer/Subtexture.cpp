#include "pch.hpp"

#include "Engine/Renderer/Subtexture.hpp"

namespace ProjectA {
	/*ref<Subtexture> Subtexture::createSprite(const ref<Texture>& texture, const vec2<float32>& position, const vec2<float32>& size, const vec2<float32>& unit) {
		// TODO: Size getter in Texture
		const vec2<float32> textureSize = {static_cast<float32>(texture->getWidth()), static_cast<float32>(texture->getHeight())};
		const vec2<float32> min = position * size / textureSize;
		const vec2<float32> max = (position + unit) * size / textureSize;

		return createRef<Subtexture>(texture, min, max);
	}

	// Coordinates order:
	// 1 2
	// 0 3
	Subtexture::Subtexture(const ref<Texture>& texture, const vec2<float32>& min, const vec2<float32>& max) :
		texture(texture),
		coordinates{
			{min[0], min[1]},
			{min[0], max[1]},
			{max[0], max[1]},
			{max[0], min[1]},
		}
	{}*/
}