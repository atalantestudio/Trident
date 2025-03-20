#pragma once

#include "Engine/Renderer/TextureFormat.hpp"
#include "Engine/Renderer/TextureType.hpp"
#include "Engine/Renderer/TextureUsage.hpp"
#include "Engine/Renderer/TextureView.hpp"

namespace ProjectA {
	struct TextureDescriptor {
		TextureType type;
		TextureFormat format;
		TextureUsage usage;
		uint32 width;
		uint32 height;
		uint32 depth;
		uint32 mipLevelCount;
		uint32 arrayLayerCount;
		uint32 sampleCount;
	};

	class Texture {
		public:
			virtual ~Texture() = default;

			// TODO: Use vec2<sint32>.
			virtual uint32 getWidth() const = 0;
			virtual uint32 getHeight() const = 0;

			virtual ref<TextureView> createView(const TextureViewDescriptor& descriptor) const = 0;
	};
}