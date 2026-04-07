// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include "Geometry.hpp"
#include "Hull.hpp"

namespace Trident {
	void EncodeUnitCircleGeometryAsset(const std::filesystem::path& outputDirectory) {
		constexpr Hull2DType HULL_TYPE = Hull2DType::CIRCLE;

		// TODO(debug-only)
		constexpr uint32 identifier = 34;

		std::vector<float32> vertexComponentBuffer;
		std::vector<uint32> indexBuffer;

		const Geometry2D geometry = createUnitCircleGeometry(32, vertexComponentBuffer, indexBuffer);
		const Hull2D hull = createHull<HULL_TYPE>(geometry, vertexComponentBuffer, indexBuffer);
		const range<uint32, uint32> lineIndexRange = hull.getLineIndexRange();

		GeometryAssetHeader geometryHeader{};
		geometryHeader.signature = AssetSignature::GEOMETRY;
		geometryHeader.identifier = identifier;
		geometryHeader.version = {0, 0, 0};
		geometryHeader.flags = GeometryAssetFlags::IS_2D | GeometryAssetFlags::HAS_HULL;
		geometryHeader.vertexCount = geometry.getVertexPositionComponentRange().count / 2;
		geometryHeader.indexCount = static_cast<uint32>(geometry.getIndexRange().count);

		Hull2DHeader hullHeader{};
		hullHeader.hullType = HULL_TYPE;
		hullHeader.centroid = hull.getCentroid();
		hullHeader.radius = hull.getRadius();
		hullHeader.lineIndexCount = lineIndexRange.count;

		const uint64 size =
			sizeof(geometryHeader) +
			geometryHeader.vertexCount * 2 * sizeof(svec2<float32>) +
			geometryHeader.indexCount * sizeof(uint32) +
			sizeof(hullHeader) +
			lineIndexRange.count * sizeof(uint32);

		ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

		std::copy_n(pointer_cast<const uint8*>(&geometryHeader), sizeof(geometryHeader), stream.getBytePosition());

		stream.jump(sizeof(geometryHeader));

		if (vertexComponentBuffer.size() > 0) {
			std::copy(vertexComponentBuffer.begin(), vertexComponentBuffer.end(), pointer_cast<float32*>(stream.getBytePosition()));

			stream.jump(vertexComponentBuffer.size() * sizeof(float32));
		}

		if (geometryHeader.indexCount > 0) {
			std::copy_n(&indexBuffer[0], geometryHeader.indexCount, pointer_cast<uint32*>(stream.getBytePosition()));

			stream.jump(geometryHeader.indexCount * sizeof(uint32));
		}

		std::copy_n(pointer_cast<const uint8*>(&hullHeader), sizeof(hullHeader), stream.getBytePosition());

		stream.jump(sizeof(hullHeader));

		if (lineIndexRange.count > 0) {
			std::copy(&indexBuffer[lineIndexRange.offset], &indexBuffer[lineIndexRange.offset + lineIndexRange.count - 1], pointer_cast<uint32*>(stream.getBytePosition()));

			stream.jump(lineIndexRange.count * sizeof(uint32));
		}

		const std::filesystem::path outputPath = outputDirectory / "UnitCircle.tga";
		const std::string outputPathString = outputPath.string();

		Writer writer = Filesystem::openWriter(view<char8>(&outputPathString[0], outputPathString.size()));
		writer.write(stream.data, stream.size);

		ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", outputPathString, identifier);
	}

	void EncodeUnitSquareGeometryAsset(const std::filesystem::path& outputDirectory) {
		constexpr Hull2DType HULL_TYPE = Hull2DType::POLYGON;

		// TODO(debug-only)
		constexpr uint32 identifier = 35;

		std::vector<float32> vertexComponentBuffer;
		std::vector<uint32> indexBuffer;

		const Geometry2D geometry = createUnitSquareGeometry(vertexComponentBuffer, indexBuffer);
		const Hull2D hull = createHull<HULL_TYPE>(geometry, vertexComponentBuffer, indexBuffer);
		const range<uint32, uint32> lineIndexRange = hull.getLineIndexRange();

		GeometryAssetHeader geometryHeader{};
		geometryHeader.signature = AssetSignature::GEOMETRY;
		geometryHeader.identifier = identifier;
		geometryHeader.version = {0, 0, 0};
		geometryHeader.flags = GeometryAssetFlags::IS_2D | GeometryAssetFlags::HAS_HULL;
		geometryHeader.vertexCount = geometry.getVertexPositionComponentRange().count / 2;
		geometryHeader.indexCount = static_cast<uint32>(geometry.getIndexRange().count);

		Hull2DHeader hullHeader{};
		hullHeader.hullType = HULL_TYPE;
		hullHeader.centroid = hull.getCentroid();
		hullHeader.radius = hull.getRadius();
		hullHeader.lineIndexCount = lineIndexRange.count;

		const uint64 size =
			sizeof(geometryHeader) +
			geometryHeader.vertexCount * 2 * sizeof(svec2<float32>) +
			geometryHeader.indexCount * sizeof(uint32) +
			sizeof(hullHeader) +
			lineIndexRange.count * sizeof(uint32);

		ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

		std::copy_n(pointer_cast<const uint8*>(&geometryHeader), sizeof(geometryHeader), stream.getBytePosition());

		stream.jump(sizeof(geometryHeader));

		std::copy(vertexComponentBuffer.begin(), vertexComponentBuffer.end(), pointer_cast<float32*>(stream.getBytePosition()));

		stream.jump(vertexComponentBuffer.size() * sizeof(float32));

		std::copy_n(&indexBuffer[0], geometryHeader.indexCount, pointer_cast<uint32*>(stream.getBytePosition()));

		stream.jump(geometryHeader.indexCount * sizeof(uint32));

		std::copy_n(pointer_cast<const uint8*>(&hullHeader), sizeof(hullHeader), stream.getBytePosition());

		stream.jump(sizeof(hullHeader));

		std::copy(&indexBuffer[lineIndexRange.offset], &indexBuffer[lineIndexRange.offset + lineIndexRange.count - 1], pointer_cast<uint32*>(stream.getBytePosition()));

		stream.jump(lineIndexRange.count * sizeof(uint32));

		const std::filesystem::path outputPath = outputDirectory / "UnitSquare.tga";
		const std::string outputPathString = outputPath.string();

		Writer writer = Filesystem::openWriter(view<char8>(&outputPathString[0], outputPathString.size()));
		writer.write(stream.data, stream.size);

		ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", outputPathString, identifier);
	}
}