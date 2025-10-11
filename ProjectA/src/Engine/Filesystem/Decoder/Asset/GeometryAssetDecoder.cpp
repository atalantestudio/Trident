#include "pch.hpp"

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
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

		const uint32 vertexPositionCount = byteStream.read<uint32>();

		geometryAssetDescriptor.vertexPositionComponents = sequence<float32>(vertexPositionCount * Geometry3D::VERTEX_POSITION_STRIDE);

		for (float32& vertexPositionComponent : geometryAssetDescriptor.vertexPositionComponents) {
			vertexPositionComponent = byteStream.read<float32>();
		}

		const uint32 vertexNormalCount = byteStream.read<uint32>();

		geometryAssetDescriptor.vertexNormalComponents = sequence<float32>(vertexNormalCount * Geometry3D::VERTEX_NORMAL_STRIDE);

		for (float32& vertexNormalComponent : geometryAssetDescriptor.vertexNormalComponents) {
			vertexNormalComponent = byteStream.read<float32>();
		}

		const uint32 vertexTextureCoordinateCount = byteStream.read<uint32>();

		geometryAssetDescriptor.vertexTextureCoordinateComponents = sequence<float32>(vertexTextureCoordinateCount * Geometry3D::VERTEX_TEXTURE_COORDINATE_STRIDE);

		for (float32& vertexTextureCoordinateComponent : geometryAssetDescriptor.vertexTextureCoordinateComponents) {
			vertexTextureCoordinateComponent = byteStream.read<float32>();
		}

		const uint32 largestVertexPositionIndex = vertexPositionCount - 1;
		const uint32 largestVertexNormalIndex = vertexNormalCount - 1;
		const uint32 largestVertexTextureCoordinateIndex = vertexTextureCoordinateCount - 1;

		const uint8 vertexPositionIndexBitCount = calculateBitCount(largestVertexPositionIndex);
		const uint8 vertexNormalIndexBitCount = calculateBitCount(largestVertexNormalIndex);
		const uint8 vertexTextureCoordinateIndexBitCount = calculateBitCount(largestVertexTextureCoordinateIndex);

		const uint32 indexCount = byteStream.read<uint32>();

		geometryAssetDescriptor.vertexPositionIndices = sequence<uint32>(indexCount);
		geometryAssetDescriptor.vertexTextureCoordinateIndices = sequence<uint32>(indexCount);
		geometryAssetDescriptor.vertexNormalIndices = sequence<uint32>(indexCount);

		BitStream<StreamOperation::READ> bitStream(byteStream.getBytePosition(), byteStream.getRemainingSize(), Endianness::LITTLE_ENDIAN);

		uint32 indexOffset = 0;

		while (indexOffset < indexCount) {
			geometryAssetDescriptor.vertexPositionIndices[indexOffset] = bitStream.read<uint32>(vertexPositionIndexBitCount);

			indexOffset += 1;
		}

		indexOffset = 0;

		while (indexOffset < indexCount) {
			geometryAssetDescriptor.vertexNormalIndices[indexOffset] = bitStream.read<uint32>(vertexNormalIndexBitCount);

			indexOffset += 1;
		}

		indexOffset = 0;

		while (indexOffset < indexCount) {
			geometryAssetDescriptor.vertexTextureCoordinateIndices[indexOffset] = bitStream.read<uint32>(vertexTextureCoordinateIndexBitCount);

			indexOffset += 1;
		}

		return createRef<GeometryAsset>(geometryAssetDescriptor);
	}
}