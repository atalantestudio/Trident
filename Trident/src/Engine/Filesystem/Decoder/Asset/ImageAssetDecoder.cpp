// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	ImageAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		ImageAssetHeader imageAssetHeader{};
		imageAssetHeader.identifier = stream.read<uint32>();
		imageAssetHeader.version = stream.read<VersionNumber>();
		imageAssetHeader.format = stream.read<uint32>();
		imageAssetHeader.extent.width = stream.read<uint32>();
		imageAssetHeader.extent.height = stream.read<uint32>();
		imageAssetHeader.extent.depth = stream.read<uint32>();
		imageAssetHeader.mipLevelCount = stream.read<uint32>();
		imageAssetHeader.layerCount = stream.read<uint32>();

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

		return ImageAsset(descriptor);
	}
}