// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	ref<MaterialAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		MaterialAsset materialAsset{};
		materialAsset.header.identifier = stream.read<uint32>();
		materialAsset.header.version = stream.read<VersionNumber>();
		materialAsset.shaderModuleName = sequence<char8>(stream.read<uint32>());

		std::copy_n(pointer_cast<const char8*>(stream.getBytePosition()), materialAsset.shaderModuleName.count(), &materialAsset.shaderModuleName[0]);

		materialAsset.stage = stream.read<ShaderStage>();
		materialAsset.entryPoint = sequence<char8>(stream.read<uint32>());

		std::copy_n(pointer_cast<const char8*>(stream.getBytePosition()), materialAsset.entryPoint.count(), &materialAsset.entryPoint[0]);

		materialAsset.descriptorBindingTypes = sequence<DescriptorType>(stream.read<uint32>());

		std::copy_n(pointer_cast<const DescriptorType*>(stream.getBytePosition()), materialAsset.descriptorBindingTypes.count(), &materialAsset.descriptorBindingTypes[0]);

		materialAsset.descriptorImages = sequence<MaterialDescriptorImage>(stream.read<uint32>());

		for (MaterialDescriptorImage& materialDescriptorImage : materialAsset.descriptorImages) {
			materialDescriptorImage.imageAsset = assetLibrary->getAsset<ImageAsset>(stream.read<uint32>());
			materialDescriptorImage.mipLevelOffset = stream.read<uint32>();
			materialDescriptorImage.mipLevelCount = stream.read<uint32>();
			materialDescriptorImage.layerOffset = stream.read<uint32>();
			materialDescriptorImage.layerCount = stream.read<uint32>();
		}

		return createRef<MaterialAsset>(materialAsset);
	}
}