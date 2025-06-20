#pragma once

#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"

namespace ProjectA {
	class ImageView;
	struct ImageViewDescriptor;

	class Image {
		public:
			Image() {}

			virtual ~Image() = default;

			ImageFormat getFormat() const {
				return format;
			}

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

			void setLayout(ImageLayout layout) {
				this->layout = layout;
			}

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) const {
				return createViewInternal(this, descriptor);
			}

		protected:
			explicit Image(const ImageDescriptor& descriptor) :
				format(descriptor.format),
				extent(descriptor.extent),
				mipLevelCount(descriptor.mipLevelCount),
				layerCount(descriptor.layerCount),
				sampleCount(descriptor.sampleCount)
			{}

			virtual ref<ImageView> createViewInternal(const Image* image, const ImageViewDescriptor& descriptor) const = 0;

		protected:
			ImageFormat format;
			Extent3D extent;
			uint32 mipLevelCount;
			uint32 layerCount;
			uint8 sampleCount : 7;
			ImageLayout layout = ImageLayout::UNDEFINED;
	};
}