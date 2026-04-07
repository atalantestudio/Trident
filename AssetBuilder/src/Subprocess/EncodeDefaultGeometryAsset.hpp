// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	inline void EncodeDefaultGeometryAsset(const std::filesystem::path& outputDirectory) {
		constexpr Hull2DType HULL_TYPE = Hull2DType::POLYGON;

		constexpr uint32 identifier = 484634187;
		const Geometry2D geometry = Geometry2D::unitSquare();
		const Hull2D hull = Hull2D::fromGeometry<HULL_TYPE>(geometry);

		GeometryAssetHeader geometryHeader{};
		geometryHeader.signature = AssetSignature::GEOMETRY;
		geometryHeader.identifier = identifier;
		geometryHeader.version = {0, 0, 0};
		geometryHeader.flags = GeometryAssetFlags::IS_2D | GeometryAssetFlags::HAS_HULL;
		geometryHeader.vertexCount = geometry.getVertexCount();
		geometryHeader.indexCount = static_cast<uint32>(geometry.getIndices().count());

		Hull2DHeader hullHeader{};
		hullHeader.hullType = HULL_TYPE;
		hullHeader.centroid = hull.getCentroid();
		hullHeader.radius = hull.getRadius();

		// NOTE: For this example the vertices on the hull are the same as the geometry.
		hullHeader.vertexCount = 0;

		hullHeader.lineIndexCount = static_cast<uint32>(hull.getLineIndices().count());

		const uint64 size =
			sizeof(geometryHeader) +
			geometry.getVertexCount() * 4 * sizeof(float32) +
			geometry.getIndexCount() * sizeof(uint32) +
			sizeof(hullHeader) +
			hull.getLineIndices().count() * sizeof(uint32);

		ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

		std::copy_n(pointer_cast<const uint8*>(&geometryHeader), sizeof(geometryHeader), stream.getBytePosition());

		stream.jump(sizeof(geometryHeader));

		std::copy(geometry.getVertexPositionComponents().begin(), geometry.getVertexPositionComponents().end(), pointer_cast<float32*>(stream.getBytePosition()));

		stream.jump(geometry.getVertexCount() * 2 * sizeof(float32));

		std::copy(geometry.getVertexTextureCoordinateComponents().begin(), geometry.getVertexTextureCoordinateComponents().end(), pointer_cast<float32*>(stream.getBytePosition()));

		stream.jump(geometry.getVertexCount() * 2 * sizeof(float32));

		std::copy(geometry.getIndices().begin(), geometry.getIndices().end(), pointer_cast<uint32*>(stream.getBytePosition()));

		stream.jump(geometry.getIndexCount() * sizeof(uint32));

		std::copy_n(pointer_cast<const uint8*>(&hullHeader), sizeof(hullHeader), stream.getBytePosition());

		stream.jump(sizeof(hullHeader));

		std::copy(hull.getLineIndices().begin(), hull.getLineIndices().begin(), pointer_cast<uint32*>(stream.getBytePosition()));

		stream.jump(hull.getLineIndices().count() * sizeof(uint32));

		const std::filesystem::path outputPath = outputDirectory / "unit_square.tga";
		const std::string outputPathString = outputPath.string();

		Writer writer = Filesystem::openWriter(view<char8>(&outputPathString[0], outputPathString.size()));
		writer.write(stream.data, stream.size);

		ENGINE_INFO("Generated default geometry asset \"[]\" (ID: []).", outputPathString, identifier);
	}
}