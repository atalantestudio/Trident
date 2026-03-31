// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"
#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	inline void EncodeDefaultGeometryAsset(/* const std::filesystem::directory_entry& directoryEntry, const std::filesystem::path& assetOutputPath */) {
		const Geometry2D geometry = Geometry2D::unitSquare();
		const Hull2D hull = Hull2D::fromGeometry<Hull2DType::POLYGON>(geometry);

		const uint32 identifier = AssetLibrary::createAssetIdentifier();

		uint64 size = 0;

		ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

		// TODO: Implement.
		ENGINE_ERROR_UNIMPLEMENTED();

		// ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", outputPath, geometryAssetIdentifier);
	}
}