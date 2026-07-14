// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	ref<MaterialAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();
		asset.shaderIndex = stream.read<uint32>();

		const uint32 descriptorImageCount = stream.read<uint32>();

		asset.descriptorImages = sequence<MaterialDescriptorImage>(descriptorImageCount);

		for (MaterialDescriptorImage& materialDescriptorImage : asset.descriptorImages) {
			const uint32 imageAssetIdentifier = stream.read<uint32>();

			materialDescriptorImage.imageAsset = assetLibrary->getAsset<ImageAsset>(imageAssetIdentifier);
			materialDescriptorImage.mipLevelOffset = stream.read<uint32>();
			materialDescriptorImage.mipLevelCount = stream.read<uint32>();
			materialDescriptorImage.layerOffset = stream.read<uint32>();
			materialDescriptorImage.layerCount = stream.read<uint32>();
		}

		return createRef<MaterialAsset>(asset);
	}
}