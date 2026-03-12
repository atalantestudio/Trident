// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"

namespace Trident {
	class ImageView {
		public:
			~ImageView();

			Image* getImage();

			uint32 getMipLevelOffset() const;

			uint32 getMipLevelCount() const;

			uint32 getLayerOffset() const;

			uint32 getLayerCount() const;

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