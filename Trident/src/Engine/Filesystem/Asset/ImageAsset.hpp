// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageFormat.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct ImageAssetDescriptor : public Asset {
		/// The format and type of the texel blocks that will be contained in the image.
		ImageFormat format;

		/// The number of texels in each dimension of the base level of the image.
		Extent3D extent;

		// TODO(asset/image): The mip level count is not handled at the moment and is always set to 1.
		/// The number of LOD available for minified sampling of the image.
		uint32 mipLevelCount;

		/// The number of layers in the image.
		uint32 layerCount;

		/// The uncompressed image data.
		sequence<uint8> data;
	};

	struct ImageAsset : public Asset {
		inline ImageFormat getFormat() const {
			return format;
		}

		inline const Extent3D& getExtent() const {
			return extent;
		}

		/// Returns the byte size of the image data.
		inline uint64 getSize() const {
			return getByteSize(format) * extent.width * extent.height * extent.depth * layerCount;
		}

		inline uint32 getMipLevelCount() const {
			return mipLevelCount;
		}

		inline uint32 getLayerCount() const {
			return layerCount;
		}

		inline const sequence<uint8>& getData() const {
			return data;
		}

		ImageFormat format;
		Extent3D extent;
		uint32 mipLevelCount;
		uint32 layerCount;
		sequence<uint8> data;
	};
}