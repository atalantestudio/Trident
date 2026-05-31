// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	ref<ImageAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, const AssetLibrary* assetLibrary) {
		const ImageAssetHeader& imageAssetHeader = *pointer_cast<const ImageAssetHeader*>(stream.getBytePosition());

		stream.jump(sizeof(imageAssetHeader));

		ImageAssetDescriptor descriptor{};
		descriptor.identifier = imageAssetHeader.identifier;
		descriptor.version = imageAssetHeader.version;
		descriptor.format = static_cast<ImageFormat>(imageAssetHeader.format);
		descriptor.extent = imageAssetHeader.extent;
		descriptor.mipLevelCount = imageAssetHeader.mipLevelCount;
		descriptor.layerCount = imageAssetHeader.layerCount;

		const uint8 channelCount = getChannelCount(descriptor.format);

		descriptor.data = sequence<uint8>(descriptor.extent.width * descriptor.extent.height * descriptor.extent.depth * descriptor.layerCount * channelCount);

		switch (descriptor.format) {
			case ImageFormat::R8_UNORM: {
				std::copy_n(stream.getBytePosition(), descriptor.data.count(), &descriptor.data[0]);

				stream.jump(descriptor.data.count());

				break;
			}
			case ImageFormat::R8G8B8_UNORM:
			case ImageFormat::R8G8B8A8_UNORM: {
				Filesystem::decodeQoiContent(stream, descriptor.data, channelCount, 0);

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		return createRef<ImageAsset>(descriptor);
	}
}