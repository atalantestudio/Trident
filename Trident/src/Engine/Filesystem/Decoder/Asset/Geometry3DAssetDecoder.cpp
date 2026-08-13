// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace Trident {
	template<>
	Geometry3DAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Geometry3DAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();
		asset.flags = stream.read<GeometryAssetFlags>();

		ENGINE_ASSERT(asset.flags & GeometryAssetFlags::IS_3D, "Geometry isn't 3D.");

		const uint32 vertexCount = stream.read<uint32>();

		asset.vertices = sequence<Vertex3D>(vertexCount);
		asset.vertexNormals = sequence<svec3<float32>>(vertexCount);

		const uint8 indexByteSize = stream.read<uint8>();

		ENGINE_ASSERT(indexByteSize == 4, "Only 32-bit indices are supported.");

		const uint32 indexCount = stream.read<uint32>();

		asset.indices = sequence<uint32>(indexCount);

		if (asset.flags & GeometryAssetFlags::HAS_HULL) {
			asset.hullType = stream.read<Hull3DType>();
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
		}

		for (Vertex3D& vertex : asset.vertices) {
			vertex.position.x = stream.read<float32>();
			vertex.position.y = stream.read<float32>();
			vertex.position.z = stream.read<float32>();
			vertex.normal = stream.read<uint32>();
			vertex.tangent = stream.read<uint32>();
			vertex.textureCoordinate = stream.read<uint32>();
		}

		for (svec3<float32>& normal : asset.vertexNormals) {
			normal.x = stream.read<float32>();
			normal.y = stream.read<float32>();
			normal.z = stream.read<float32>();
		}

		for (uint32& index : asset.indices) {
			index = stream.read<uint32>();
		}

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

		return asset;
	}
}