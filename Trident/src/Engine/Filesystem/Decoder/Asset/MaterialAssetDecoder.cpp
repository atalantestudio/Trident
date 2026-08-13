// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	MaterialAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		MaterialAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();
		asset.shaderModuleAssetIdentifier = stream.read<uint32>();

		const uint32 descriptorImageAssetIdentifierCount = stream.read<uint32>();

		asset.descriptorImageAssetIdentifiers = sequence<uint32>(descriptorImageAssetIdentifierCount);

		for (uint32& descriptorImageAssetIdentifier : asset.descriptorImageAssetIdentifiers) {
			descriptorImageAssetIdentifier = stream.read<uint32>();
		}

		return asset;
	}
}