#pragma once

#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class Image {
		protected:
			Image(const ImageDescriptor& descriptor) :
				extent(descriptor.extent),
				mipLevelCount(descriptor.mipLevelCount),
				layerCount(descriptor.layerCount),
				sampleCount(descriptor.sampleCount)
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

			uint8 getSampleCount() const {
				return sampleCount;
			}

			ImageLayout getLayout() const {
				return layout;
			}

			virtual ref<ImageView> createView(const ImageViewDescriptor& descriptor) const = 0;

		protected:
			const Extent3D extent;
			const uint32 mipLevelCount;
			const uint32 layerCount;
			const uint8 sampleCount : 7;

			ImageLayout layout = ImageLayout::UNDEFINED;
	};
}