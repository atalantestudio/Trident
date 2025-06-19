#pragma once

#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"
#include "Engine/Validation/Descriptor.hpp"

namespace ProjectA {
	class ImageView {
		public:
			virtual ~ImageView() = default;

			const Image* getImage() const {
				return image;
			}

			uint32 getMipLevelOffset() const {
				return mipLevelOffset;
			}

			uint32 getMipLevelCount() const {
				return mipLevelCount;
			}

			uint32 getLayerOffset() const {
				return layerOffset;
			}

			uint32 getLayerCount() const {
				return layerCount;
			}

		protected:
			explicit ImageView(const Image* image, const ImageViewDescriptor& descriptor) :
				image(image),
				mipLevelOffset(descriptor.mipLevelOffset),
				mipLevelCount(descriptor.mipLevelCount),
				layerOffset(descriptor.layerOffset),
				layerCount(descriptor.layerCount)
			{
				DEBUG_VALIDATE_DESCRIPTOR(descriptor);
			}

		private:
			const Image* image;
			uint32 mipLevelOffset;
			uint32 mipLevelCount;
			uint32 layerOffset;
			uint32 layerCount;
	};
}