// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Geometry/Geometry2D.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class Geometry2DProcess : public Process {
		private:
			static ByteStream<StreamOperation::WRITE> encodeGeometryAsset(uint32 identifier, const BuildGeometry2D& geometry) {
				const range<uint32, uint32> positionComponentRange = geometry.geometry.getVertexPositionComponentRange();
				const range<uint32, uint32> textureCoordinateComponentRange = geometry.geometry.getVertexTextureCoordinateComponentRange();
				const range<uint32, uint32> indexRange = geometry.geometry.getIndexRange();
				const uint64 streamSize = calculateStreamSize(geometry);

				ByteStream<StreamOperation::WRITE> stream(streamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::GEOMETRY);
				stream.write<uint32>(identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<GeometryAssetFlags>(geometry.flags);
				stream.write<uint32>(positionComponentRange.count / 2);
				stream.write<uint32>(textureCoordinateComponentRange.count / 2);
				stream.write<uint32>(indexRange.count);

				if (geometry.flags & GeometryAssetFlags::HAS_HULL) {
					stream.write<svec2<float32>>(geometry.hull.centroid);
					stream.write<float32>(geometry.hull.radius);
					stream.write<uint32>(geometry.hull.lineIndexRange.count);
					stream.write<Hull2DType>(geometry.hull.type);
				}

				uint32 positionComponentIndex = 0;

				while (positionComponentIndex < positionComponentRange.count) {
					const float32 positionComponent = geometry.vertexComponentBuffer[positionComponentRange.offset + positionComponentIndex];

					stream.write<float32>(positionComponent);

					positionComponentIndex += 1;
				}

				uint32 textureCoordinateComponentIndex = 0;

				while (textureCoordinateComponentIndex < textureCoordinateComponentRange.count) {
					const float32 textureCoordinateComponent = geometry.vertexComponentBuffer[textureCoordinateComponentRange.offset + textureCoordinateComponentIndex];

					stream.write<float32>(textureCoordinateComponent);

					textureCoordinateComponentIndex += 1;
				}

				uint32 indexOffset = 0;

				while (indexOffset < indexRange.count) {
					const uint32 index = geometry.indexBuffer[indexRange.offset + indexOffset];

					stream.write<uint32>(index);

					indexOffset += 1;
				}

				if (geometry.flags & GeometryAssetFlags::HAS_HULL) {
					uint32 hullLineIndexOffset = 0;

					while (hullLineIndexOffset < geometry.hull.lineIndexRange.count) {
						const uint32 hullLineIndex = geometry.indexBuffer[geometry.hull.lineIndexRange.offset + hullLineIndexOffset];

						stream.write<uint32>(hullLineIndex);

						hullLineIndexOffset += 1;
					}
				}

				return stream;
			}

			static uint64 calculateStreamSize(const BuildGeometry2D& geometry) {
				uint64 streamSize = (
					sizeof(AssetHeader) +
					sizeof(GeometryAssetFlags) +
					sizeof(uint32) * 3 +
					geometry.geometry.getVertexPositionComponentRange().count * sizeof(float32) +
					geometry.geometry.getVertexTextureCoordinateComponentRange().count * sizeof(float32) +
					geometry.geometry.getIndexRange().count * sizeof(uint32)
				);

				if (geometry.flags & GeometryAssetFlags::HAS_HULL) {
					streamSize += (
						sizeof(svec2<float32>) +
						sizeof(float32) +
						sizeof(uint32) +
						sizeof(Hull2DType) +
						geometry.hull.lineIndexRange.count * sizeof(uint32)
					);
				}

				return streamSize;
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Geometry/";
				const view<char8> assetName = getAssetName();

				std::filesystem::path outputPath = assetOutputPath / std::string(assetName);
				outputPath.replace_extension("tga");

				if (std::filesystem::exists(outputPath)) {
					return 0;
				}

				const uint32 assetIdentifier = AssetLibrary::createAssetIdentifier();
				const BuildGeometry2D geometry = createGeometry();
				const ByteStream<StreamOperation::WRITE> stream = encodeGeometryAsset(assetIdentifier, geometry);

				createGeometryAsset(assetIdentifier, stream, assetName, assetOutputPath);

				ENGINE_INFO("Generated geometry asset \"[]\" (ID: []).", assetName, assetIdentifier);

				return 0;
			}

		protected:
			explicit Geometry2DProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			virtual view<char8> getAssetName() const = 0;

			virtual BuildGeometry2D createGeometry() const = 0;

		private:
			void createGeometryAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tga", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.getByteOffset());

				assetLibrary->registerAsset(identifier, assetName);
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}