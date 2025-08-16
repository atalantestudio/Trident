#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace ProjectA {
	struct ImageAssetDescriptor : public AssetDescriptor {
		// The format and type of the texel blocks that will be contained in the image.
		ImageFormat format;

		// The number of texels in each dimension of the base level of the image.
		Extent3D extent;

		// The number of LOD available for minified sampling of the image.
		uint32 mipLevelCount;

		// The number of layers in the image.
		uint32 layerCount;

		// Uncompressed image data.
		sequence<uint8> data;
	};

	// An image stored in CPU memory (as opposed to Image).
	class ImageAsset : public Asset {
		public:
			explicit ImageAsset(const ImageAssetDescriptor& descriptor) :
				Asset(descriptor),
				format(descriptor.format),
				extent(descriptor.extent),
				mipLevelCount(descriptor.mipLevelCount),
				layerCount(descriptor.layerCount),
				data(descriptor.data)
			{}

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

			const sequence<uint8>& getData() const {
				return data;
			}

		private:
			ImageFormat format;
			Extent3D extent;
			uint32 mipLevelCount;
			uint32 layerCount;
			sequence<uint8> data;
	};
}