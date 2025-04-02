#pragma once

#include "Engine/Renderer/ImageFormat.hpp"
#include "Engine/Renderer/ImageType.hpp"
#include "Engine/Renderer/ImageUsage.hpp"
#include "Engine/Renderer/ImageView.hpp"

namespace ProjectA {
	struct ImageDescriptor {
		ImageType type;
		ImageFormat format;
		ImageUsage usage;
		uint32 width;
		uint32 height;
		uint32 depth;
		uint32 mipLevelCount;
		uint32 arrayLayerCount;
		uint32 sampleCount;
	};

	class Image {
		public:
			virtual ~Image() = default;

			// TODO: Use vec2<sint32>.
			virtual uint32 getWidth() const = 0;
			virtual uint32 getHeight() const = 0;

			virtual ref<ImageView> createView(const ImageViewDescriptor& descriptor) const = 0;
	};
}