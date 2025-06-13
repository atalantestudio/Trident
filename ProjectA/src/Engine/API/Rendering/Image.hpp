#pragma once

#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/Extent.hpp"

namespace ProjectA {
	class Image {
		protected:
			Image(const ImageDescriptor& descriptor) :
				extent{descriptor.width, descriptor.height, descriptor.depth},
				mipLevelCount(descriptor.mipLevelCount),
				layerCount(descriptor.layerCount)
			{}

		public:
			virtual ~Image() = default;

			const Extent3D& getExtent() const {
				return extent;
			}

			uint32 getMipLevelCount() const {
				return mipLevelCount;
			}

			uint32 getLayerCount() const {
				return layerCount;
			}

			virtual ref<ImageView> createView(const ImageViewDescriptor& descriptor) const = 0;

		protected:
			const Extent3D extent;
			const uint32 mipLevelCount;
			const uint32 layerCount;
	};
}