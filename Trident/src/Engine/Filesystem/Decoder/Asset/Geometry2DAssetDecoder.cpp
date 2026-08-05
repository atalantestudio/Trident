// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	ref<Geometry2DAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		Geometry2DAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();
		asset.flags = stream.read<GeometryAssetFlags>();

		ENGINE_ASSERT(!(asset.flags & GeometryAssetFlags::IS_3D), "Geometry isn't 2D.");

		const uint32 positionCount = stream.read<uint32>();

		asset.vertexPositionComponents = sequence<float32>(positionCount * 2);

		const uint32 textureCoordinateCount = stream.read<uint32>();

		asset.vertexTextureCoordinateComponents = sequence<float32>(textureCoordinateCount * 2);

		const uint32 indexCount = stream.read<uint32>();

		asset.vertexPositionIndices = sequence<uint32>(indexCount);

		if (asset.flags & GeometryAssetFlags::HAS_HULL) {
			asset.hullCentroid = stream.read<svec2<float32>>();
			asset.hullRadius = stream.read<float32>();

			const uint32 hullLineIndexCount = stream.read<uint32>();

			asset.hullLineIndices = sequence<uint32>(hullLineIndexCount);
			asset.hullType = stream.read<Hull2DType>();
		}

		for (float32& positionComponent : asset.vertexPositionComponents) {
			positionComponent = stream.read<float32>();
		}

		for (float32& textureCoordinateComponent : asset.vertexTextureCoordinateComponents) {
			textureCoordinateComponent = stream.read<float32>();
		}

		for (uint32& positionIndex : asset.vertexPositionIndices) {
			positionIndex = stream.read<uint32>();
		}

		for (uint32& hullLineIndex : asset.hullLineIndices) {
			hullLineIndex = stream.read<uint32>();
		}

		return createRef<Geometry2DAsset>(asset);
	}
}