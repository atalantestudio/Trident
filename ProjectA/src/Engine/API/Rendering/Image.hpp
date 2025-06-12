#pragma once

#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/Extent.hpp"

namespace ProjectA {
	class Image {
		protected:
			Image(const ImageDescriptor& descriptor) :
				extent{descriptor.width, descriptor.height, descriptor.depth}
			{}

		public:
			virtual ~Image() = default;

			Extent3D getExtent() const {
				return extent;
			}

			virtual ref<ImageView> createView(const ImageViewDescriptor& descriptor) const = 0;

		protected:
			const Extent3D extent;
	};
}