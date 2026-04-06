// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"

namespace Trident {
	class Image {
		public:
			~Image();

			/// Returns the type of the image.
			inline ImageType getType() const {
				return type;
			}

			/// Returns the format of the image.
			inline ImageFormat getFormat() const {
				return format;
			}

			/// Returns the 3-dimensional size of the image.
			inline Extent3D getExtent() const {
				return extent;
			}

			/// Returns the byte size of the image data.
			inline uint64 getSize() const {
				return getChannelCount(format) * extent.width * extent.height * extent.depth * layerCount;
			}

			/// Returns the number of mip levels in the image.
			inline uint32 getMipLevelCount() const {
				return mipLevelCount;
			}

			/// Returns the number of layers in the image.
			inline uint32 getLayerCount() const {
				return layerCount;
			}

			/// Returns the number of samples in the image.
			inline uint8 getSampleCount() const {
				return sampleCount;
			}

			inline ImageUsage getUsage() const {
				return usage;
			}

			/// Returns the current layout of the image.
			inline ImageLayout getLayout() const {
				return layout;
			}

			/// Returns the current access flags for the image.
			inline Access getAccess() const {
				return access;
			}

			ref<ImageView> createView(const ImageViewDescriptor& descriptor);

		protected:
			explicit Image(const Device* device, const ImageDescriptor& descriptor);

		// TODO: Don't expose to the client side.
		public:
			const Device* device;

			ImageType type;
			ImageFormat format;
			Extent3D extent;
			uint32 mipLevelCount;
			uint32 layerCount;
			uint8 sampleCount;
			ImageUsage usage;
			ImageLayout layout = ImageLayout::UNDEFINED;
			Access access = Access::NONE;
	};
}