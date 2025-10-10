#include "pch.hpp"

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace ProjectA {
	template<>
	ref<GeometryAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& byteStream) {
		GeometryAssetDescriptor geometryAssetDescriptor{};
		geometryAssetDescriptor.identifier = byteStream.read<uint32>();
		geometryAssetDescriptor.version = byteStream.read<VersionNumber>();
		geometryAssetDescriptor.clusterCount = byteStream.read<uint32>();

		const uint32 vertexCount = byteStream.read<uint32>();
		const uint32 vertexPositionComponentCount = vertexCount * 3;

		geometryAssetDescriptor.vertexPositionComponents = sequence<float32>(vertexPositionComponentCount);

		for (uint32 vertexPositionComponentIndex = 0; vertexPositionComponentIndex < vertexPositionComponentCount; vertexPositionComponentIndex += 1) {
			geometryAssetDescriptor.vertexPositionComponents[vertexPositionComponentIndex] = byteStream.read<float32>();
		}

		const uint32 indexCount = byteStream.read<uint32>();
		const uint32 largestIndex = vertexCount - 1;
		const uint8 indexBitCount = calculateBitCount(largestIndex);

		geometryAssetDescriptor.vertexPositionIndices = sequence<uint32>(indexCount);

		BitStream<StreamOperation::READ> bitStream(byteStream.getBytePosition(), byteStream.getRemainingSize(), Endianness::LITTLE_ENDIAN);

		for (uint32 indexOffset = 0; indexOffset < indexCount; indexOffset += 1) {
			geometryAssetDescriptor.vertexPositionIndices[indexOffset] = bitStream.read<uint32>(indexBitCount);
		}

		return createRef<GeometryAsset>(geometryAssetDescriptor);
	}
}