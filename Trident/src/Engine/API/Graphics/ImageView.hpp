// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Descriptor/ImageViewDescriptor.hpp"

namespace Trident {
	class ImageView {
		public:
			~ImageView();

			inline Image* getImage() {
				return image;
			}

			inline uint32 getMipLevelOffset() const {
				return mipLevelOffset;
			}

			inline uint32 getMipLevelCount() const {
				return mipLevelCount;
			}

			inline uint32 getLayerOffset() const {
				return layerOffset;
			}

			inline uint32 getLayerCount() const {
				return layerCount;
			}

		protected:
			explicit ImageView(const Device* device, Image* image, const ImageViewDescriptor& descriptor);

		// TODO: Don't expose to the client side.
		public:
			const Device* device;
			Image* image;

			uint32 mipLevelOffset;
			uint32 mipLevelCount;
			uint32 layerOffset;
			uint32 layerCount;
	};
}