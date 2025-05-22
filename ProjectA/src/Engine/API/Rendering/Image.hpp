#pragma once

#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class Image {
		public:
			virtual ~Image() = default;

			// TODO: Use vec2<sint32>.
			virtual uint32 getWidth() const = 0;
			virtual uint32 getHeight() const = 0;

			virtual ref<ImageView> createView(const ImageViewDescriptor& descriptor) const = 0;
	};
}