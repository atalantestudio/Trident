// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace Trident {
	class Image;
	class ImageView;
	struct ImageViewDescriptor;

	// An image stored in GPU memory (as opposed to ImageAsset).
	class Image {
		public:
			Image() {}

			virtual ~Image() = default;

			/// Returns the type of the image.
			ImageType getType() const {
				return type;
			}

			/// Returns the format of the image.
			ImageFormat getFormat() const {
				return format;
			}

			/// Returns the 3-dimensional size of the image.
			const Extent3D& getExtent() const {
				return extent;
			}

			/// Returns the number of mip levels in the image.
			uint32 getMipLevelCount() const {
				return mipLevelCount;
			}

			/// Returns the number of layers in the image.
			uint32 getLayerCount() const {
				return layerCount;
			}

			/// Returns the number of samples in the image.
			uint8 getSampleCount() const {
				return sampleCount;
			}

			/// Returns the current layout of the image.
			ImageLayout getLayout() const {
				return layout;
			}

			// TODO: Remove (don't expose to the client side).
			void setLayout(ImageLayout layout) {
				this->layout = layout;
			}

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) {
				return createViewInternal(descriptor, this);
			}

		protected:
			explicit Image(const ImageDescriptor& descriptor) :
				type(descriptor.type),
				format(descriptor.format),
				extent(descriptor.extent),
				mipLevelCount(descriptor.mipLevelCount),
				layerCount(descriptor.layerCount),
				sampleCount(descriptor.sampleCount)
			{
				DEBUG_DESCRIPTOR_GUARD(descriptor);
			}

			virtual ref<ImageView> createViewInternal(const ImageViewDescriptor& descriptor, Image* image) = 0;

		protected:
			ImageType type;
			ImageFormat format;
			Extent3D extent;
			uint32 mipLevelCount;
			uint32 layerCount;
			uint8 sampleCount;
			ImageLayout layout = ImageLayout::UNDEFINED;
	};
}