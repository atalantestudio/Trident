// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"

namespace Trident {
	// TODO(architecture): Move inside utility folder?
	uint32 calculateMaxMipLevelCount(Extent3D extent);

	class Image {
		public:
			~Image();

			/// Returns the type of the image.
			ImageType getType() const;

			/// Returns the format of the image.
			ImageFormat getFormat() const;

			/// Returns the 3-dimensional size of the image.
			Extent3D getExtent() const;

			/// Returns the byte size of the image data.
			uint64 getSize() const;

			/// Returns the number of mip levels in the image.
			uint32 getMipLevelCount() const;

			/// Returns the number of layers in the image.
			uint32 getLayerCount() const;

			/// Returns the number of samples in the image.
			uint8 getSampleCount() const;

			/// Returns the image usage flags.
			ImageUsage getUsage() const;

			/// Returns the current layout of the image.
			ImageLayout getLayout() const;

			/// Returns the current access flags for the image.
			Access getAccess() const;

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