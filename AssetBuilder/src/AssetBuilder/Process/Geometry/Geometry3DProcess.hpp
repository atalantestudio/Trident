// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Geometry/Geometry3D.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class Geometry3DProcess : public Process {
		public:
			static ByteStream<StreamOperation::WRITE> encodeGeometryAsset(uint32 identifier, const BuildGeometry3D& geometry) {
				const uint64 streamSize = calculateStreamSize(geometry);

				ByteStream<StreamOperation::WRITE> stream(streamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::GEOMETRY);
				stream.write<uint32>(identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<GeometryAssetFlags>(geometry.flags);
				stream.write<uint32>(static_cast<uint32>(geometry.vertices.count()));
				stream.write<uint8>(sizeof(uint32));
				stream.write<uint32>(static_cast<uint32>(geometry.indices.count()));

				if (geometry.flags & GeometryAssetFlags::HAS_HULL) {
					stream.write<Hull3DType>(geometry.hull.type);
					stream.write<float32>(geometry.hull.centroid.x);
					stream.write<float32>(geometry.hull.centroid.y);
					stream.write<float32>(geometry.hull.centroid.z);
					stream.write<float32>(geometry.hull.radius);
					stream.write<uint16>(static_cast<uint16>(geometry.hullEdges.count()));
					stream.write<uint16>(static_cast<uint16>(geometry.hullFaces.count()));
					stream.write<uint16>(static_cast<uint16>(geometry.hullFaceEdgeIndices.count()));
					stream.write<uint16>(static_cast<uint16>(geometry.hullLineIndices.count()));
				}

				for (const BuildVertex3D& vertex : geometry.vertices) {
					stream.write<float32>(vertex.position.x);
					stream.write<float32>(vertex.position.y);
					stream.write<float32>(vertex.position.z);
					stream.write<uint32>(pack<VertexInputAttributeFormat::R10G10B10A2_UNORM, uint32>(vertex.normal));
					stream.write<uint32>(pack<VertexInputAttributeFormat::R10G10B10A2_UNORM, uint32>(vertex.tangent));
					stream.write<uint32>(pack<VertexInputAttributeFormat::R16G16_FLOAT, uint32>(vertex.textureCoordinate));
				}

				for (const BuildVertex3D& vertex : geometry.vertices) {
					stream.write<float32>(vertex.normal.x);
					stream.write<float32>(vertex.normal.y);
					stream.write<float32>(vertex.normal.z);
				}

				for (const uint32 index : geometry.indices) {
					stream.write<uint32>(index);
				}

				if (geometry.flags & GeometryAssetFlags::HAS_HULL) {
					for (const Edge3D edge : geometry.hullEdges) {
						stream.write<Edge3D>(edge);
					}

					for (const Plane3D& face : geometry.hullFaces) {
						stream.write<float32>(face.normal.x);
						stream.write<float32>(face.normal.y);
						stream.write<float32>(face.normal.z);
						stream.write<float32>(face.distance);
						stream.write<range<uint16, uint16>>(face.edgeIndexRange);
					}

					for (const uint16 faceEdgeIndex : geometry.hullFaceEdgeIndices) {
						stream.write<uint16>(faceEdgeIndex);
					}

					for (const uint16 lineIndex : geometry.hullLineIndices) {
						stream.write<uint16>(lineIndex);
					}
				}

				return stream;
			}

		private:
			static uint64 calculateStreamSize(const BuildGeometry3D& geometry) {
				uint64 streamSize = (
					sizeof(AssetHeader) +
					sizeof(GeometryAssetFlags) +
					sizeof(uint8) +
					sizeof(uint32) * 2 +
					geometry.vertices.count() * (sizeof(Vertex3D) + sizeof(svec3<float32>)) +
					geometry.indices.count() * sizeof(uint32)
				);

				if (geometry.flags & GeometryAssetFlags::HAS_HULL) {
					streamSize += (
						sizeof(Hull3DType) +
						sizeof(svec3<float32>) +
						sizeof(float32) +
						sizeof(uint16) * 4 +
						geometry.hullEdges.count() * sizeof(Edge3D) +
						geometry.hullFaces.count() * sizeof(Plane3D) +
						geometry.hullFaceEdgeIndices.count() * sizeof(uint16) +
						geometry.hullLineIndices.count() * sizeof(uint16)
					);
				}

				return streamSize;
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Geometry/";
				const view<char8> assetName = getAssetName();

				std::filesystem::path finalAssetOutputPath = assetOutputPath / std::string(assetName);
				finalAssetOutputPath.replace_extension("tga");

				if (std::filesystem::exists(finalAssetOutputPath)) {
					ENGINE_INFO("Geometry asset \"[]\" already exists.", finalAssetOutputPath.filename().string());

					return 0;
				}

				const uint32 assetIdentifier = AssetLibrary::createAssetIdentifier();
				const BuildGeometry3D geometry = createGeometry();
				const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset(assetIdentifier, geometry);

				createGeometryAsset(assetIdentifier, stream, assetName, assetOutputPath);

				ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", assetName, assetIdentifier);

				return 0;
			}

		protected:
			explicit Geometry3DProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			virtual view<char8> getAssetName() const = 0;

			virtual BuildGeometry3D createGeometry() const = 0;

		private:
			void createGeometryAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tga", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				assetLibrary->registerAsset(identifier, assetName);
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}