#pragma once

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/File/FBXFile.hpp"

namespace ProjectA {
	static void EncodeGeometryAssetSubprocess(const std::filesystem::directory_entry& directoryEntry, const std::string& assetOutputPath) {
		const std::string path = directoryEntry.path().string();
		const sequence<uint8> inputData = Filesystem::openBuffer(path);

		if (!Filesystem::validateSignature<FileFormat::FBX>(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count, Endianness::LITTLE_ENDIAN))) {
			ENGINE_INFO("Ignoring non-FBX file \"[]\".", path);

			return;
		}

		const File<FileFormat::FBX> file = Filesystem::decode<FileFormat::FBX>(inputData);

		const FBXNode* ObjectsNode = file.searchNodeRecursive(file.NodeList, "Objects");
		const FBXNode* VerticesNode = file.searchNodeRecursive(ObjectsNode->NestedList, "Vertices");
		const FBXNode* PolygonVertexIndexNode = file.searchNodeRecursive(ObjectsNode->NestedList, "PolygonVertexIndex");

		// Assume vertex position components are stored in an array property.
		const sequence<float64>& vertexPositionComponents = std::static_pointer_cast<FBXArrayProperty<float64>>(VerticesNode->PropertyList[0])->Contents;

		ENGINE_ASSERT(vertexPositionComponents.count >= 3, "Invalid vertex position component buffer length.");

		// Assume vertex position indices are stored in an array property.
		const sequence<sint32>& vertexPositionIndices = std::static_pointer_cast<FBXArrayProperty<sint32>>(PolygonVertexIndexNode->PropertyList[0])->Contents;

		ENGINE_ASSERT(vertexPositionIndices.count >= 3, "Invalid vertex position index buffer length.");

		uint32 indexCount = 0;

		// Get vertex position index buffer size.
		{
			uint32 polygonOffset = 0;
			uint32 triangleOffset = 0;

			while (polygonOffset < vertexPositionIndices.count) {
				while (triangleOffset < vertexPositionIndices.count - polygonOffset) {
					const sint32 thirdIndex = vertexPositionIndices[polygonOffset + triangleOffset + 2];

					indexCount += 3;

					// A negative index means it's the last index in the current polygon.
					if (thirdIndex < 0) {
						polygonOffset += triangleOffset + 2;

						break;
					}

					triangleOffset += 1;
				}

				polygonOffset += 1;
				triangleOffset = 0;
			}
		}

		const uint32 geometryAssetIdentifier = AssetLibrary::createAssetIdentifier();
		const uint32 clusterCount = static_cast<uint32>(ceil(static_cast<float32>(indexCount) / CLUSTER_INDEX_COUNT));
		const uint32 vertexCount = static_cast<uint32>(vertexPositionComponents.count) / 3;

		const uint32 largestIndex = vertexCount - 1;
		const uint8 indexBitCount = calculateBitCount(largestIndex);
		const uint64 totalIndexByteCount = static_cast<uint64>(ceil((indexCount * indexBitCount) / 8.0));

		// TODO: Use 16-bit indices when possible (?)
		const uint64 outputSize =
			sizeof(AssetSignature) +
			sizeof(VersionNumber) +
			sizeof(uint32) * 4 +
			sizeof(float32) * vertexCount * 3 +
			totalIndexByteCount;

		ByteStream<StreamOperation::WRITE> byteStream(outputSize, Endianness::LITTLE_ENDIAN);
		byteStream.write<AssetSignature>(AssetSignature::TGE);
		byteStream.write<uint32>(geometryAssetIdentifier);
		byteStream.write<VersionNumber>({0, 0, 0});
		byteStream.write<uint32>(clusterCount);
		byteStream.write<uint32>(vertexCount);

		// TODO: Find a way to truncate the bits of each vertex position component.
		for (uint32 vertexPositionComponentIndex = 0; vertexPositionComponentIndex < vertexPositionComponents.count; vertexPositionComponentIndex += 1) {
			byteStream.write<float32>(static_cast<float32>(vertexPositionComponents[vertexPositionComponentIndex]));
		}

		byteStream.write<uint32>(indexCount);

		BitStream<StreamOperation::WRITE> bitStream(byteStream.getBytePosition(), byteStream.getRemainingSize(), Endianness::LITTLE_ENDIAN);

		uint32 polygonCount = 0;
		uint32 polygonOffset = 0;
		uint32 triangleOffset = 0;
		uint32 vertexOffset = 0;

		// Write vertex position indices.
		while (polygonOffset < vertexPositionIndices.count) {
			while (triangleOffset < vertexPositionIndices.count - polygonOffset) {
				const sint32 indexA = vertexPositionIndices[polygonOffset];
				const sint32 indexB = vertexPositionIndices[polygonOffset + triangleOffset + 1];
				const sint32 indexC = vertexPositionIndices[polygonOffset + triangleOffset + 2];

				bitStream.write<uint32>(indexA, indexBitCount);
				bitStream.write<uint32>(indexB, indexBitCount);

				vertexOffset += 3;

				if (indexC < 0) {
					bitStream.write<uint32>(~indexC, indexBitCount);

					polygonOffset += triangleOffset + 2;
					polygonCount += 1;

					break;
				}

				bitStream.write<uint32>(indexC, indexBitCount);

				triangleOffset += 1;
			}

			polygonOffset += 1;
			triangleOffset = 0;
		}

		const std::string outputPath = assetOutputPath + "Geometry/" + directoryEntry.path().stem().string() + ".tge";

		Writer writer = Filesystem::openWriter(outputPath);
		writer.write(byteStream.data, byteStream.size);

		ENGINE_INFO("Generated geometry asset (ID: []).", geometryAssetIdentifier);
	}
}