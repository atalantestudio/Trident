// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	ImageAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		ImageAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();
		asset.format = stream.read<ImageFormat>();
		asset.extent = {stream.read<uint32>(), stream.read<uint32>(), stream.read<uint32>()};
		asset.mipLevelCount = stream.read<uint32>();
		asset.layerCount = stream.read<uint32>();

		const uint8 channelCount = getChannelCount(asset.format);

		asset.data = sequence<uint8>(getByteSize(asset.format) * asset.extent.width * asset.extent.height * asset.extent.depth * asset.layerCount);

		switch (asset.format) {
			case ImageFormat::R8_UNORM: {
				std::copy_n(stream.getBytePosition(), asset.data.count(), &asset.data[0]);

				stream.jump(asset.data.count());

				break;
			}
			case ImageFormat::R8G8B8_UNORM:
			case ImageFormat::R8G8B8A8_UNORM: {
				Filesystem::decodeQoiContent(stream, asset.data, channelCount, 0);

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		return asset;
	}
}