// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	static ShaderModuleAsset decodeLegacyShaderModuleAsset(ByteStream<StreamOperation::READ>& stream, ShaderModuleAsset& asset) {
		const uint32 nameCharacterCount = stream.read<uint32>();

		asset.name = sequence<char8>(nameCharacterCount);
		asset.stageFlags = stream.read<ShaderStage>();

		const uint32 entryPointCharacterCount = stream.read<uint32>();

		asset.entryPoint = sequence<char8>(entryPointCharacterCount);

		const uint32 bindingCount = stream.read<uint32>();

		asset.bindings = sequence<DescriptorSetLayoutBinding>(bindingCount);

		const uint32 descriptorSetCount = stream.read<uint32>();

		asset.descriptorSetBindingRanges = sequence<range<uint32, uint32>>(descriptorSetCount);

		std::copy_n(pointer_cast<const char8*>(stream.getBytePosition()), nameCharacterCount, &asset.name[0]);

		stream.jump(nameCharacterCount);

		std::copy_n(pointer_cast<const char8*>(stream.getBytePosition()), entryPointCharacterCount, &asset.entryPoint[0]);

		stream.jump(entryPointCharacterCount);

		for (DescriptorSetLayoutBinding& binding : asset.bindings) {
			binding.descriptorType = stream.read<DescriptorType>();
			binding.descriptorCount = stream.read<uint32>();
			binding.shaderStages = stream.read<ShaderStage>();
		}

		uint32 bindingOffset = 0;

		for (range<uint32, uint32>& descriptorSetBindingRange : asset.descriptorSetBindingRanges) {
			descriptorSetBindingRange.offset = bindingOffset;
			descriptorSetBindingRange.count = stream.read<uint32>();

			bindingOffset += descriptorSetBindingRange.count;
		}

		return asset;
	}

	template<>
	ShaderModuleAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		ShaderModuleAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();

		// TODO(tsl): Remove once TSL is ready.
		if (asset.version.major == 0) {
			return decodeLegacyShaderModuleAsset(stream, asset);
		}

		// TODO(tsl): Update with the latest revision of ShaderModuleAsset.
		ENGINE_ERROR_UNIMPLEMENTED();

		asset.glslCode = sequence<char8>(stream.read<uint32>());

		for (char8& character : asset.glslCode) {
			character = stream.read<char8>();
		}

		const uint32 entryPointCount = stream.read<uint32>();

		asset.glslEntryPointActivationOffsets.reserve(entryPointCount);

		sequence<view<char8>> entryPointIdentifiers(entryPointCount);
		uint32 entryPointIndex = 0;

		while (entryPointIndex < entryPointCount) {
			const uint32 identifierSize = stream.read<uint32>();

			entryPointIdentifiers[entryPointIndex] = view<char8>(pointer_cast<const char8*>(stream.getBytePosition()), identifierSize);

			stream.jump(identifierSize);

			entryPointIndex += 1;
		}

		entryPointIndex = 0;

		while (entryPointIndex < entryPointCount) {
			const uint32 activationOffset = stream.read<uint32>();

			asset.glslEntryPointActivationOffsets.insert({entryPointIdentifiers[entryPointIndex], activationOffset});

			entryPointIndex += 1;
		}

		return asset;
	}
}