// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	template<>
	ref<ShaderModuleAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, const AssetLibrary* assetLibrary) {
		ShaderModuleAssetDescriptor descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();
		descriptor.glslCode = sequence<char8>(stream.read<uint32>());

		for (char8& character : descriptor.glslCode) {
			character = stream.read<char8>();
		}

		const uint32 entryPointCount = stream.read<uint32>();

		descriptor.glslEntryPointActivationOffsets.reserve(entryPointCount);

		uint32 entryPointIndex = 0;

		sequence<view<char8>> entryPointIdentifiers(entryPointCount);

		while (entryPointIndex < entryPointCount) {
			const uint32 identifierSize = stream.read<uint32>();

			entryPointIdentifiers[entryPointIndex] = view<char8>(pointer_cast<const char8*>(stream.getBytePosition()), identifierSize);

			stream.jump(identifierSize);

			entryPointIndex += 1;
		}

		entryPointIndex = 0;

		while (entryPointIndex < entryPointCount) {
			const uint32 activationOffset = stream.read<uint32>();

			descriptor.glslEntryPointActivationOffsets.insert({entryPointIdentifiers[entryPointIndex], activationOffset});

			entryPointIndex += 1;
		}

		return createRef<ShaderModuleAsset>(descriptor);
	}
}