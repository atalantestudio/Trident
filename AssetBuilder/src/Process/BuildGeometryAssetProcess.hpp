// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "../Subprocess/GeometryData.hpp"
#include "../Subprocess/DefaultGeometries.hpp"
#include "../Subprocess/EncodeGeometryAssetSubprocess.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Stream/BitStream.hpp"

namespace Trident {
	class BuildGeometryAssetProcess final : public Process {
		private:
			typedef Geometry2DData (*CreateGeometry2DFunction)();
			typedef Geometry3DData (*CreateGeometry3DFunction)();

		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

			static constexpr uint32 DEFAULT_GEOMETRY_2D_COUNT = 4;
			static constexpr uint32 DEFAULT_GEOMETRY_3D_COUNT = 4;

			inline static const view<char8> DEFAULT_GEOMETRY_2D_NAMES[DEFAULT_GEOMETRY_2D_COUNT] = {
				"UnitCircle",
				"UnitSquare",
				"Triangle",
				"Polygon",
			};

			inline static constexpr CreateGeometry2DFunction DEFAULT_GEOMETRY_2D_FUNCTIONS[DEFAULT_GEOMETRY_2D_COUNT] = {
				createUnitCircle,
				createUnitSquare,
				createTriangle,
				createPolygon,
			};

			inline static const view<char8> DEFAULT_GEOMETRY_3D_NAMES[DEFAULT_GEOMETRY_3D_COUNT] = {
				"UnitBox",
				"UnitPlane",
				"RectangularCuboid",
				"SingleTextureUnitBox",
			};

			inline static constexpr CreateGeometry3DFunction DEFAULT_GEOMETRY_3D_FUNCTIONS[DEFAULT_GEOMETRY_3D_COUNT] = {
				createUnitBox,
				createUnitPlane,
				createRectangularCuboid,
				createSingleTextureUnitBox,
			};

		private:
			static uint64 calculateStreamSize(const Geometry2DData& data) {
				uint64 streamSize = (
					sizeof(AssetHeader) +
					sizeof(GeometryAssetFlags) +
					sizeof(uint32) * 2 +
					data.vertexComponentBuffer.size() * sizeof(float32) +
					data.indexBuffer.size() * sizeof(uint32)
				);

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					streamSize += (
						sizeof(svec2<float32>) +
						sizeof(float32) +
						sizeof(uint32) +
						sizeof(Hull2DType) +
						data.hull.lineIndexRange.count * sizeof(uint32)
					);
				}

				return streamSize;
			}

			static uint64 calculateStreamSize(const Geometry3DData& data, uint64 totalIndexByteCount) {
				uint64 streamSize = (
					sizeof(AssetHeader) +
					sizeof(GeometryAssetFlags) +
					sizeof(uint32) * 5 +
					data.positions.count() * sizeof(svec3<float32>) +
					data.normals.count() * sizeof(svec3<float32>) +
					data.textureCoordinates.count() * sizeof(svec2<float32>) +
					data.tangents.count() * sizeof(svec3<float32>) +
					totalIndexByteCount
				);

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					streamSize += (
						sizeof(svec3<float32>) +
						sizeof(float32) +
						sizeof(uint16) * 4 +
						sizeof(Hull3DType) +
						data.hullEdges.count() * sizeof(Edge3D) +
						data.hullFaces.count() * sizeof(Plane3D) +
						data.hullFaceEdgeIndices.count() * sizeof(uint16) +
						data.hullLineIndices.count() * sizeof(uint16)
					);
				}

				return streamSize;
			}

			static ByteStream<StreamOperation::WRITE> encodeGeometryAsset2D(uint32 identifier, const Geometry2DData& data) {
				const uint64 streamSize = calculateStreamSize(data);

				ByteStream<StreamOperation::WRITE> stream(streamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::GEOMETRY);
				stream.write<uint32>(identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<GeometryAssetFlags>(data.flags);
				stream.write<uint32>(static_cast<uint32>(data.vertexComponentBuffer.size() / 2));
				stream.write<uint32>(static_cast<uint32>(data.indexBuffer.size()));

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					stream.write<svec2<float32>>(data.hull.centroid);
					stream.write<float32>(data.hull.radius);
					stream.write<uint32>(data.hull.lineIndexRange.count);
					stream.write<Hull2DType>(data.hull.type);
				}

				for (const float32 vertexComponent : data.vertexComponentBuffer) {
					stream.write<float32>(vertexComponent);
				}

				for (const uint32 index : data.indexBuffer) {
					stream.write<uint32>(index);
				}

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					uint32 hullLineIndexOffset = 0;

					while (hullLineIndexOffset < data.hull.lineIndexRange.count) {
						const uint32 hullLineIndex = data.indexBuffer[data.hull.lineIndexRange.offset + hullLineIndexOffset];

						stream.write<uint32>(hullLineIndex);

						hullLineIndexOffset += 1;
					}
				}

				return stream;
			}

			static ByteStream<StreamOperation::WRITE> encodeGeometryAsset3D(uint32 identifier, const Geometry3DData& data) {
				const uint64 positionCount = data.positions.count();
				const uint64 normalCount = data.normals.count();
				const uint64 textureCoordinateCount = data.textureCoordinates.count();
				const uint64 indexCount = data.positionIndices.count();
				const uint8 positionIndexBitCount = countBits(positionCount - 1);
				const uint8 normalIndexBitCount = countBits(normalCount - 1);
				const uint8 textureCoordinateIndexBitCount = countBits(textureCoordinateCount - 1);
				const uint64 totalIndexByteCount = static_cast<uint64>(ceil(indexCount * (positionIndexBitCount + normalIndexBitCount + textureCoordinateIndexBitCount) / 8.0f));

				const uint64 streamSize = calculateStreamSize(data, totalIndexByteCount);

				ByteStream<StreamOperation::WRITE> stream(streamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::GEOMETRY);
				stream.write<uint32>(identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<GeometryAssetFlags>(data.flags);
				stream.write<uint32>(static_cast<uint32>(data.positions.count()));
				stream.write<uint32>(static_cast<uint32>(data.normals.count()));
				stream.write<uint32>(static_cast<uint32>(data.textureCoordinates.count()));
				stream.write<uint32>(static_cast<uint32>(data.tangents.count()));
				stream.write<uint32>(static_cast<uint32>(data.positionIndices.count()));

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					stream.write<svec3<float32>>(data.hull.centroid);
					stream.write<float32>(data.hull.radius);
					stream.write<uint16>(static_cast<uint16>(data.hullEdges.count()));
					stream.write<uint16>(static_cast<uint16>(data.hullFaces.count()));
					stream.write<uint16>(static_cast<uint16>(data.hullFaceEdgeIndices.count()));
					stream.write<uint16>(static_cast<uint16>(data.hullLineIndices.count()));
					stream.write<Hull3DType>(data.hull.type);
				}

				for (const svec3<float32> position : data.positions) {
					stream.write<float32>(position.x);
					stream.write<float32>(position.y);
					stream.write<float32>(position.z);
				}

				for (const svec3<float32> normal : data.normals) {
					stream.write<float32>(normal.x);
					stream.write<float32>(normal.y);
					stream.write<float32>(normal.z);
				}

				for (const svec2<float32> textureCoordinate : data.textureCoordinates) {
					stream.write<svec2<float32>>(textureCoordinate);
				}

				for (const svec3<float32> tangent : data.tangents) {
					stream.write<float32>(tangent.x);
					stream.write<float32>(tangent.y);
					stream.write<float32>(tangent.z);
				}

				{
					BitStream<StreamOperation::WRITE> bitStream(stream.getBytePosition(), totalIndexByteCount, Endianness::LITTLE_ENDIAN);

					for (const uint32 positionIndex : data.positionIndices) {
						bitStream.write<uint32>(positionIndex, positionIndexBitCount);
					}

					for (const uint32 normalIndex : data.normalIndices) {
						bitStream.write<uint32>(normalIndex, normalIndexBitCount);
					}

					for (const uint32 textureCoordinateIndex : data.textureCoordinateIndices) {
						bitStream.write<uint32>(textureCoordinateIndex, textureCoordinateIndexBitCount);
					}
				}

				stream.jump(totalIndexByteCount);

				if (data.flags & GeometryAssetFlags::HAS_HULL) {
					for (const Edge3D edge : data.hullEdges) {
						stream.write<Edge3D>(edge);
					}

					for (const Plane3D& face : data.hullFaces) {
						stream.write<float32>(face.normal.x);
						stream.write<float32>(face.normal.y);
						stream.write<float32>(face.normal.z);
						stream.write<float32>(face.distance);
						stream.write<range<uint16, uint16>>(face.edgeIndexRange);
					}

					for (const uint16 faceEdgeIndex : data.hullFaceEdgeIndices) {
						stream.write<uint16>(faceEdgeIndex);
					}

					for (const uint16 lineIndex : data.hullLineIndices) {
						stream.write<uint16>(lineIndex);
					}
				}

				return stream;
			}

			void writeGeometryAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tga", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", outputPath, identifier);

				assetLibrary->registerAsset(identifier, assetName);
			}

		public:
			explicit BuildGeometryAssetProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = static_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Geometry/";
				const std::filesystem::recursive_directory_iterator geometryDirectoryIterator(rootInputPath / "Asset/Geometry");

				uint32 defaultGeometryIndex = 0;

				while (defaultGeometryIndex < DEFAULT_GEOMETRY_2D_COUNT) {
					std::filesystem::path finalAssetOutputPath = assetOutputPath / std::string(DEFAULT_GEOMETRY_2D_NAMES[defaultGeometryIndex]);
					finalAssetOutputPath.replace_extension("tga");

					if (std::filesystem::exists(finalAssetOutputPath)) {
						ENGINE_INFO("2D geometry asset \"[]\" already exists.", finalAssetOutputPath.filename().string());

						defaultGeometryIndex += 1;

						continue;
					}

					const Geometry2DData geometryData = DEFAULT_GEOMETRY_2D_FUNCTIONS[defaultGeometryIndex]();
					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset2D(identifier, geometryData);

					writeGeometryAsset(identifier, stream, DEFAULT_GEOMETRY_2D_NAMES[defaultGeometryIndex], assetOutputPath);

					defaultGeometryIndex += 1;
				}

				defaultGeometryIndex = 0;

				while (defaultGeometryIndex < DEFAULT_GEOMETRY_3D_COUNT) {
					std::filesystem::path finalAssetOutputPath = assetOutputPath / std::string(DEFAULT_GEOMETRY_3D_NAMES[defaultGeometryIndex]);
					finalAssetOutputPath.replace_extension("tga");

					if (std::filesystem::exists(finalAssetOutputPath)) {
						ENGINE_INFO("3D geometry asset \"[]\" already exists.", finalAssetOutputPath.filename().string());

						defaultGeometryIndex += 1;

						continue;
					}

					const Geometry3DData geometryData = DEFAULT_GEOMETRY_3D_FUNCTIONS[defaultGeometryIndex]();
					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset3D(identifier, geometryData);

					writeGeometryAsset(identifier, stream, DEFAULT_GEOMETRY_3D_NAMES[defaultGeometryIndex], assetOutputPath);

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

					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const std::string assetName = directoryEntry.path().stem().string();
					const Geometry3DData geometryData = DecodeFBXGeometry(inputData, identifier);
					const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset3D(identifier, geometryData);

					writeGeometryAsset(identifier, stream, view<char8>(&assetName[0], assetName.size()), assetOutputPath);
				}

				ENGINE_INFO("Done building geometry assets.");

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}