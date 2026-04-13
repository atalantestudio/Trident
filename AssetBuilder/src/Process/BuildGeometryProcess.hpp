// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"

#include "../Subprocess/GeometryData.hpp"
#include "../Subprocess/DefaultGeometries.hpp"
#include "../Subprocess/EncodeGeometryAssetSubprocess.hpp"

namespace Trident {
	class BuildGeometryProcess final : public Process {
		private:
			typedef Geometry2DData (*CreateGeometry2DFunction)();
			typedef Geometry3DData (*CreateGeometry3DFunction)();

		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

			static constexpr uint32 DEFAULT_GEOMETRY_2D_COUNT = 2;
			static constexpr uint32 DEFAULT_GEOMETRY_3D_COUNT = 1;

			inline static const view<char8> DEFAULT_GEOMETRY_2D_NAMES[DEFAULT_GEOMETRY_2D_COUNT] = {
				"UnitCircle",
				"UnitSquare",
			};

			inline static constexpr CreateGeometry2DFunction DEFAULT_GEOMETRY_2D_FUNCTIONS[DEFAULT_GEOMETRY_2D_COUNT] = {
				createUnitCircle,
				createUnitSquare,
			};

			inline static const view<char8> DEFAULT_GEOMETRY_3D_NAMES[DEFAULT_GEOMETRY_3D_COUNT] = {
				"UnitBox",
			};

			inline static constexpr CreateGeometry3DFunction DEFAULT_GEOMETRY_3D_FUNCTIONS[DEFAULT_GEOMETRY_3D_COUNT] = {
				createUnitBox,
			};

		private:
			static ByteStream<StreamOperation::WRITE> encodeGeometryAsset2D(uint32 identifier, const Geometry2DData& data) {
				GeometryAsset2DHeader geometryHeader{};
				geometryHeader.signature = AssetSignature::GEOMETRY;
				geometryHeader.identifier = identifier;
				geometryHeader.version = {0, 0, 0};
				geometryHeader.flags = GeometryAssetFlags::IS_2D | GeometryAssetFlags::HAS_HULL;
				geometryHeader.vertexCount = data.geometry.getVertexPositionComponentRange().count / 2;
				geometryHeader.indexCount = static_cast<uint32>(data.geometry.getIndexRange().count);

				Hull2DHeader hullHeader{};
				hullHeader.hullType = data.hull.type;
				hullHeader.centroid = data.hull.centroid;
				hullHeader.radius = data.hull.radius;
				hullHeader.lineIndexCount = data.hull.lineIndexRange.count;

				const uint64 size =
					sizeof(geometryHeader) +
					geometryHeader.vertexCount * 2 * sizeof(svec2<float32>) +
					geometryHeader.indexCount * sizeof(uint32) +
					sizeof(hullHeader) +
					data.hull.lineIndexRange.count * sizeof(uint32);

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&geometryHeader), sizeof(geometryHeader), stream.getBytePosition());

				stream.jump(sizeof(geometryHeader));

				if (data.vertexComponentBuffer.size() > 0) {
					std::copy(data.vertexComponentBuffer.begin(), data.vertexComponentBuffer.end(), pointer_cast<float32*>(stream.getBytePosition()));

					stream.jump(data.vertexComponentBuffer.size() * sizeof(float32));
				}

				if (geometryHeader.indexCount > 0) {
					std::copy_n(&data.indexBuffer[0], geometryHeader.indexCount, pointer_cast<uint32*>(stream.getBytePosition()));

					stream.jump(geometryHeader.indexCount * sizeof(uint32));
				}

				std::copy_n(pointer_cast<const uint8*>(&hullHeader), sizeof(hullHeader), stream.getBytePosition());

				stream.jump(sizeof(hullHeader));

				if (data.hull.lineIndexRange.count > 0) {
					std::copy(&data.indexBuffer[data.hull.lineIndexRange.offset], &data.indexBuffer[data.hull.lineIndexRange.offset + data.hull.lineIndexRange.count - 1], pointer_cast<uint32*>(stream.getBytePosition()));

					stream.jump(data.hull.lineIndexRange.count * sizeof(uint32));
				}

				return stream;
			}

			static ByteStream<StreamOperation::WRITE> encodeGeometryAsset3D(uint32 identifier, const Geometry3DData& data) {
				Geometry3DAssetHeader header{};
				header.signature = AssetSignature::GEOMETRY;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.flags = GeometryAssetFlags::IS_3D;

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					header.flags |= GeometryAssetFlags::HAS_HULL;
				}

				header.vertexPositionCount = static_cast<uint32>(data.vertexPositionComponents.count()) / 3;
				header.vertexNormalCount = static_cast<uint32>(data.vertexNormalComponents.count()) / 3;
				header.vertexTextureCoordinateCount = static_cast<uint32>(data.vertexTextureCoordinateComponents.count()) / 2;
				header.triangleTangentCount = static_cast<uint32>(data.triangleTangentComponents.count()) / 3;
				header.indexCount = static_cast<uint32>(data.vertexPositionIndices.count());

				Hull3DHeader hullHeader{};

				const uint8 vertexPositionIndexBitCount = countBits(header.vertexPositionCount - 1);
				const uint8 vertexNormalIndexBitCount = countBits(header.vertexNormalCount - 1);
				const uint8 vertexTextureCoordinateIndexBitCount = countBits(header.vertexTextureCoordinateCount - 1);
				const uint64 totalIndexByteCount = static_cast<uint64>(ceil(header.indexCount * (vertexPositionIndexBitCount + vertexNormalIndexBitCount + vertexTextureCoordinateIndexBitCount) / 8.0f));

				uint64 outputSize =
					sizeof(header) +
					data.vertexPositionComponents.count() * sizeof(float32) +
					data.vertexNormalComponents.count() * sizeof(float32) +
					data.vertexTextureCoordinateComponents.count() * sizeof(float32) +
					data.triangleTangentComponents.count() * sizeof(float32) +
					totalIndexByteCount;

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					hullHeader.centroid = data.hull.centroid;
					hullHeader.radius = data.hull.radius;
					hullHeader.edgeCount = static_cast<uint16>(data.hullEdges.count());
					hullHeader.faceCount = static_cast<uint16>(data.hullFaceNormalComponents.count() / 3);
					hullHeader.lineIndexCount = static_cast<uint16>(data.hullLineIndices.count());
					hullHeader.hullType = data.hull.type;

					outputSize +=
						sizeof(hullHeader) +
						data.hullEdges.count() * sizeof(HalfEdge) +
						data.hullFaceNormalComponents.count() * sizeof(float32) +
						data.hullLineIndices.count() * sizeof(uint16);
				}

				ByteStream<StreamOperation::WRITE> stream(outputSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(header.signature);
				stream.write<uint32>(header.identifier);
				stream.write<VersionNumber>(header.version);
				stream.write<GeometryAssetFlags>(header.flags);
				stream.jump(sizeof(uint8) * 3);
				stream.write<uint32>(header.vertexPositionCount);
				stream.write<uint32>(header.vertexNormalCount);
				stream.write<uint32>(header.vertexTextureCoordinateCount);
				stream.write<uint32>(header.triangleTangentCount);
				stream.write<uint32>(header.indexCount);

				for (const float64 vertexPositionComponent : data.vertexPositionComponents) {
					stream.write<float32>(static_cast<float32>(vertexPositionComponent));
				}

				for (const float64 vertexNormalComponent : data.vertexNormalComponents) {
					stream.write<float32>(static_cast<float32>(vertexNormalComponent));
				}

				for (const float64 vertexTextureCoordinateComponent : data.vertexTextureCoordinateComponents) {
					stream.write<float32>(static_cast<float32>(vertexTextureCoordinateComponent));
				}

				for (const float32 triangleTangentComponent : data.triangleTangentComponents) {
					stream.write<float32>(triangleTangentComponent);
				}

				{
					BitStream<StreamOperation::WRITE> bitStream(stream.getBytePosition(), totalIndexByteCount, Endianness::LITTLE_ENDIAN);

					for (const uint32 vertexPositionIndex : data.vertexPositionIndices) {
						bitStream.write<uint32>(vertexPositionIndex, vertexPositionIndexBitCount);
					}

					for (const uint32 vertexNormalIndex : data.vertexNormalIndices) {
						bitStream.write<uint32>(vertexNormalIndex, vertexNormalIndexBitCount);
					}

					for (const uint32 vertexTextureCoordinateIndex : data.vertexTextureCoordinateIndices) {
						bitStream.write<uint32>(vertexTextureCoordinateIndex, vertexTextureCoordinateIndexBitCount);
					}
				}

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					stream.jump(totalIndexByteCount);

					stream.write<svec3<float32>>(hullHeader.centroid);
					stream.write<float32>(hullHeader.radius);
					stream.write<uint16>(hullHeader.edgeCount);
					stream.write<uint16>(hullHeader.faceCount);
					stream.write<uint16>(hullHeader.lineIndexCount);
					stream.write<Hull3DType>(hullHeader.hullType);
					stream.jump(sizeof(uint8));

					for (const HalfEdge edge : data.hullEdges) {
						stream.write<HalfEdge>(edge);
					}

					for (const float32 faceNormalComponent : data.hullFaceNormalComponents) {
						stream.write<float32>(faceNormalComponent);
					}

					for (const uint16 lineIndex : data.hullLineIndices) {
						stream.write<uint16>(lineIndex);
					}
				}

				return stream;
			}

			static void writeGeometryAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tga", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", outputPath, identifier);
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = pointer_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path rootOutputPath = pointer_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Geometry/";
				const std::filesystem::recursive_directory_iterator geometryDirectoryIterator(rootInputPath / "Asset/Geometry");

				// TODO(debug-only)
				uint32 identifier = 34;

				uint32 defaultGeometryIndex = 0;

				while (defaultGeometryIndex < DEFAULT_GEOMETRY_2D_COUNT) {
					const Geometry2DData geometryData = DEFAULT_GEOMETRY_2D_FUNCTIONS[defaultGeometryIndex]();
					const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset2D(identifier, geometryData);

					writeGeometryAsset(identifier, stream, DEFAULT_GEOMETRY_2D_NAMES[defaultGeometryIndex], assetOutputPath);

					identifier += 1;
					defaultGeometryIndex += 1;
				}

				defaultGeometryIndex = 0;

				while (defaultGeometryIndex < DEFAULT_GEOMETRY_3D_COUNT) {
					const Geometry3DData geometryData = DEFAULT_GEOMETRY_3D_FUNCTIONS[defaultGeometryIndex]();
					const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset3D(identifier, geometryData);

					writeGeometryAsset(identifier, stream, DEFAULT_GEOMETRY_3D_NAMES[defaultGeometryIndex], assetOutputPath);

					identifier += 1;
					defaultGeometryIndex += 1;
				}

				for (const std::filesystem::directory_entry& directoryEntry : geometryDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					const sequence<char8> path = directoryEntry.path().string();
					const sequence<uint8> inputData = Filesystem::openByteBuffer(path);

					std::filesystem::path finalAssetOutputPath = assetOutputPath / directoryEntry.path().filename();
					finalAssetOutputPath.replace_extension("tga");

					if (std::filesystem::exists(finalAssetOutputPath)) {
						ENGINE_INFO("Geometry asset \"[]\" already exists.", finalAssetOutputPath.filename().string());

						continue;
					}

					if (!Decoder<FileFormat::FBX>::validateSignature(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count(), Endianness::LITTLE_ENDIAN))) {
						ENGINE_INFO("Ignoring non-FBX file \"[]\".", path);

						continue;
					}

					const std::string assetName = directoryEntry.path().stem().string();
					const Geometry3DData geometryData = DecodeFBXGeometry(inputData, identifier);
					const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset3D(identifier, geometryData);

					writeGeometryAsset(identifier, stream, view<char8>(&assetName[0], assetName.size()), assetOutputPath);

					identifier += 1;
				}

				ENGINE_INFO("Done building geometry assets.");

				return 0;
			}
	};
}