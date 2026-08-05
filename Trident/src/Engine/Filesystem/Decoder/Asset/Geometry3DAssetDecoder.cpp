// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace Trident {
	template<>
	ref<Geometry3DAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Geometry3DAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();
		asset.flags = stream.read<GeometryAssetFlags>();

		ENGINE_ASSERT(asset.flags & GeometryAssetFlags::IS_3D, "Geometry isn't 3D.");

		const uint32 positionCount = stream.read<uint32>();

		asset.positions = sequence<svec3<float32>>(positionCount);

		const uint32 normalCount = stream.read<uint32>();

		asset.normals = sequence<svec3<float32>>(normalCount);

		const uint32 textureCoordinateCount = stream.read<uint32>();

		asset.textureCoordinates = sequence<svec2<float32>>(textureCoordinateCount);

		const uint32 tangentCount = stream.read<uint32>();

		asset.tangents = sequence<svec3<float32>>(tangentCount);
		asset.indexCount = stream.read<uint32>();
		asset.positionIndices = sequence<uint32>(asset.indexCount);
		asset.textureCoordinateIndices = sequence<uint32>(asset.indexCount);
		asset.normalIndices = sequence<uint32>(asset.indexCount);

		if (asset.flags & GeometryAssetFlags::HAS_HULL) {
			asset.hullCentroid = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			asset.hullRadius = stream.read<float32>();

			const uint16 hullEdgeCount = stream.read<uint16>();

			asset.hullEdges = sequence<Edge3D>(hullEdgeCount);

			const uint16 hullFaceCount = stream.read<uint16>();

			asset.hullFaces = sequence<Plane3D>(hullFaceCount);

			const uint16 hullFaceEdgeIndexCount = stream.read<uint16>();

			asset.hullFaceEdgeIndices = sequence<uint16>(hullFaceEdgeIndexCount);

			const uint16 hullLineIndexCount = stream.read<uint16>();

			asset.hullLineIndices = sequence<uint16>(hullLineIndexCount);
			asset.hullType = stream.read<Hull3DType>();
		}

		for (svec3<float32>& position : asset.positions) {
			position.x = stream.read<float32>();
			position.y = stream.read<float32>();
			position.z = stream.read<float32>();
		}

		for (svec3<float32>& normal : asset.normals) {
			normal.x = stream.read<float32>();
			normal.y = stream.read<float32>();
			normal.z = stream.read<float32>();
		}

		for (svec2<float32>& textureCoordinate : asset.textureCoordinates) {
			textureCoordinate = stream.read<svec2<float32>>();
		}

		for (svec3<float32>& tangent : asset.tangents) {
			tangent.x = stream.read<float32>();
			tangent.y = stream.read<float32>();
			tangent.z = stream.read<float32>();
		}

		const uint8 positionIndexBitCount = countBits(positionCount - 1);
		const uint8 normalIndexBitCount = countBits(normalCount - 1);
		const uint8 textureCoordinateIndexBitCount = countBits(textureCoordinateCount - 1);
		const uint64 totalIndexByteCount = static_cast<uint64>(ceil(asset.indexCount * (positionIndexBitCount + normalIndexBitCount + textureCoordinateIndexBitCount) / 8.0f));

		{
			BitStream<StreamOperation::READ> bitStream(stream.getBytePosition(), totalIndexByteCount, Endianness::LITTLE_ENDIAN);

			uint32 indexOffset = 0;

			while (indexOffset < asset.indexCount) {
				asset.positionIndices[indexOffset] = bitStream.read<uint32>(positionIndexBitCount);

				indexOffset += 1;
			}

			indexOffset = 0;

			while (indexOffset < asset.indexCount) {
				asset.normalIndices[indexOffset] = bitStream.read<uint32>(normalIndexBitCount);

				indexOffset += 1;
			}

			indexOffset = 0;

			while (indexOffset < asset.indexCount) {
				asset.textureCoordinateIndices[indexOffset] = bitStream.read<uint32>(textureCoordinateIndexBitCount);

				indexOffset += 1;
			}
		}

		stream.jump(totalIndexByteCount);

		for (Edge3D& edge : asset.hullEdges) {
			edge = stream.read<Edge3D>();
		}

		for (Plane3D& face : asset.hullFaces) {
			face.normal = {stream.read<float32>(), stream.read<float32>(), stream.read<float32>()};
			face.distance = stream.read<float32>();
			face.edgeIndexRange = stream.read<range<uint16, uint16>>();
		}

		for (uint16& faceEdgeIndex : asset.hullFaceEdgeIndices) {
			faceEdgeIndex = stream.read<uint16>();
		}

		for (uint16& lineIndex : asset.hullLineIndices) {
			lineIndex = stream.read<uint16>();
		}

		return createRef<Geometry3DAsset>(asset);
	}
}