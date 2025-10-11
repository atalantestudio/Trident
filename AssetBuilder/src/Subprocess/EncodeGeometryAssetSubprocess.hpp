#pragma once

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/File/FBXFile.hpp"

namespace ProjectA {
	// TODO: Assert that FBX nodes exist before using them.
	static void EncodeGeometryAssetSubprocess(const std::filesystem::directory_entry& directoryEntry, const std::string& assetOutputPath) {
		const std::string path = directoryEntry.path().string();
		const sequence<uint8> inputData = Filesystem::openBuffer(path);

		if (!Filesystem::validateSignature<FileFormat::FBX>(ByteStream<StreamOperation::READ>(&inputData[0], inputData.count, Endianness::LITTLE_ENDIAN))) {
			ENGINE_INFO("Ignoring non-FBX file \"[]\".", path);

			return;
		}

		const File<FileFormat::FBX> file = Filesystem::decode<FileFormat::FBX>(inputData);

		const FBXNode* ObjectsNode = file.searchNodeRecursive(file.NodeList, "Objects");
		const FBXNode* GeometryNode = file.searchNodeRecursive(file.NodeList, "Geometry");
		const FBXNode* VerticesNode = file.searchNodeRecursive(GeometryNode->NestedList, "Vertices");
		const FBXNode* PolygonVertexIndexNode = file.searchNodeRecursive(GeometryNode->NestedList, "PolygonVertexIndex");

		DEBUG_ASSERT(VerticesNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::d);
		DEBUG_ASSERT(PolygonVertexIndexNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::i);

		const ref<FBXArrayProperty<float64>> VerticesProperty = std::static_pointer_cast<FBXArrayProperty<float64>>(VerticesNode->PropertyList[0]);
		const ref<FBXArrayProperty<sint32>> PolygonVertexIndexProperty = std::static_pointer_cast<FBXArrayProperty<sint32>>(PolygonVertexIndexNode->PropertyList[0]);

		const sequence<float64>& vertexPositionComponents = VerticesProperty->Contents;

		ENGINE_ASSERT(vertexPositionComponents.count >= 3, "Invalid vertex position component buffer length.");
		ENGINE_ASSERT(PolygonVertexIndexProperty->Contents.count >= 3, "Invalid vertex position index buffer length.");

		uint32 indexCount = 0;

		// Get vertex position index buffer size.
		{
			uint32 polygonOffset = 0;
			uint32 triangleOffset = 0;

			while (polygonOffset < PolygonVertexIndexProperty->Contents.count) {
				while (triangleOffset < PolygonVertexIndexProperty->Contents.count - polygonOffset) {
					const sint32 thirdIndex = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 2];

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

		sequence<uint32> vertexPositionIndices(indexCount);
		uint32 polygonCount = 0;
		uint32 polygonOffset = 0;
		uint32 indexOffset = 0;

		// Parse vertex position indices.
		while (polygonOffset < PolygonVertexIndexProperty->Contents.count) {
			uint32 triangleOffset = 0;

			while (triangleOffset < PolygonVertexIndexProperty->Contents.count - polygonOffset) {
				const sint32 indexA = PolygonVertexIndexProperty->Contents[polygonOffset];
				const sint32 indexB = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 1];
				const sint32 indexC = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 2];

				vertexPositionIndices[indexOffset + 0] = indexA;
				vertexPositionIndices[indexOffset + 1] = indexB;

				indexOffset += 3;

				if (indexC < 0) {
					vertexPositionIndices[indexOffset - 1] = ~indexC;

					polygonOffset += triangleOffset + 2;
					polygonCount += 1;

					break;
				}

				vertexPositionIndices[indexOffset - 1] = indexC;

				triangleOffset += 1;
			}

			polygonOffset += 1;
		}

		sequence<float64> vertexNormalComponents;
		sequence<uint32> vertexNormalIndices;

		// Parse vertex normal components and indices.
		{
			const FBXNode* LayerElementNormalNode = file.searchNodeRecursive(GeometryNode->NestedList, "LayerElementNormal");
			const FBXNode* MappingInformationTypeNode = file.searchNodeRecursive(LayerElementNormalNode->NestedList, "MappingInformationType");
			const FBXNode* ReferenceInformationTypeNode = file.searchNodeRecursive(LayerElementNormalNode->NestedList, "ReferenceInformationType");
			const FBXNode* NormalsNode = file.searchNodeRecursive(LayerElementNormalNode->NestedList, "Normals");

			DEBUG_ASSERT(MappingInformationTypeNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::S);
			DEBUG_ASSERT(ReferenceInformationTypeNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::S);
			DEBUG_ASSERT(NormalsNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::d);

			const ref<FBXSpecialProperty> MappingInformationTypeProperty = std::static_pointer_cast<FBXSpecialProperty>(MappingInformationTypeNode->PropertyList[0]);
			const ref<FBXSpecialProperty> ReferenceInformationTypeProperty = std::static_pointer_cast<FBXSpecialProperty>(ReferenceInformationTypeNode->PropertyList[0]);
			const ref<FBXArrayProperty<float64>> NormalsProperty = std::static_pointer_cast<FBXArrayProperty<float64>>(NormalsNode->PropertyList[0]);

			const std::string_view MappingInformationType(pointer_cast<const char8*>(MappingInformationTypeProperty->Data.begin()), MappingInformationTypeProperty->Data.count);
			const std::string_view ReferenceInformationType(pointer_cast<const char8*>(ReferenceInformationTypeProperty->Data.begin()), ReferenceInformationTypeProperty->Data.count);

			if (MappingInformationType == "ByPolygonVertex") {
				if (ReferenceInformationType == "Direct") {
					vertexNormalComponents = sequence<float64>(vertexPositionComponents.count);
					vertexNormalIndices = vertexPositionIndices;

					// Sum normals for each individual vertex.
					for (uint32 vertexNormalIndexOffset = 0; vertexNormalIndexOffset < PolygonVertexIndexProperty->Contents.count; vertexNormalIndexOffset += 1) {
						const uint32 vertexNormalIndex = PolygonVertexIndexProperty->Contents[vertexNormalIndexOffset] < 0 ? ~PolygonVertexIndexProperty->Contents[vertexNormalIndexOffset] : PolygonVertexIndexProperty->Contents[vertexNormalIndexOffset];

						vertexNormalComponents[vertexNormalIndex * 3 + 0] += NormalsProperty->Contents[vertexNormalIndexOffset * 3 + 0];
						vertexNormalComponents[vertexNormalIndex * 3 + 1] += NormalsProperty->Contents[vertexNormalIndexOffset * 3 + 1];
						vertexNormalComponents[vertexNormalIndex * 3 + 2] += NormalsProperty->Contents[vertexNormalIndexOffset * 3 + 2];
					}

					// Normalize the sum and overwrite.
					for (uint32 vertexNormalIndexOffset = 0; vertexNormalIndexOffset < vertexNormalComponents.count; vertexNormalIndexOffset += 3) {
						const vec3<float64> normal = normalized(vec3<float64>(
							vertexNormalComponents[vertexNormalIndexOffset + 0],
							vertexNormalComponents[vertexNormalIndexOffset + 1],
							vertexNormalComponents[vertexNormalIndexOffset + 2]
						));

						vertexNormalComponents[vertexNormalIndexOffset + 0] = normal[0];
						vertexNormalComponents[vertexNormalIndexOffset + 1] = normal[1];
						vertexNormalComponents[vertexNormalIndexOffset + 2] = normal[2];
					}
				}
				else {
					ENGINE_ERROR("Encountered unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);
					ABORT();

					return;
				}
			}
			else if (MappingInformationType == "ByVertex" || MappingInformationType == "ByVertice") {
				vertexNormalIndices = vertexPositionIndices;

				if (ReferenceInformationType == "Direct") {
					vertexNormalComponents = NormalsProperty->Contents;
				}
				else if (ReferenceInformationType == "IndexToDirect") {
					const FBXNode* NormalsIndexNode = file.searchNodeRecursive(LayerElementNormalNode->NestedList, "NormalsIndex");

					DEBUG_ASSERT(NormalsIndexNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::i);

					const ref<FBXArrayProperty<sint32>> NormalsIndexProperty = std::static_pointer_cast<FBXArrayProperty<sint32>>(NormalsIndexNode->PropertyList[0]);

					vertexNormalComponents = sequence<float64>(NormalsProperty->Contents.count);

					for (uint32 index = 0; index < NormalsIndexProperty->Contents.count; index += 1) {
						const sint32 normalIndex = NormalsIndexProperty->Contents[index];

						ENGINE_ASSERT(normalIndex >= 0, "Invalid normal index.");

						// TODO: Use std::copy here?
						for (uint32 componentIndex = 0; componentIndex < Geometry3D::VERTEX_NORMAL_STRIDE; componentIndex += 1) {
							vertexNormalComponents[index * Geometry3D::VERTEX_NORMAL_STRIDE + componentIndex] = NormalsProperty->Contents[normalIndex * Geometry3D::VERTEX_NORMAL_STRIDE + componentIndex];
						}
					}
				}
				else {
					ENGINE_ERROR("Encountered unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);
					ABORT();

					return;
				}
			}
			else {
				ENGINE_ERROR("Encountered unhandled MappingInformationType value \"[]\".", MappingInformationType);
				ABORT();

				return;
			}
		}

		ENGINE_ASSERT(vertexNormalIndices.count == vertexPositionIndices.count, "Vertex normal index count is inconsistent with vertex position index count.");

		sequence<float64> vertexTextureCoordinateComponents;
		sequence<uint32> vertexTextureCoordinateIndices;

		// Parse vertex texture coordinates.
		{
			const FBXNode* LayerElementUVNode = file.searchNodeRecursive(GeometryNode->NestedList, "LayerElementUV");
			const FBXNode* MappingInformationTypeNode = file.searchNodeRecursive(LayerElementUVNode->NestedList, "MappingInformationType");
			const FBXNode* ReferenceInformationTypeNode = file.searchNodeRecursive(LayerElementUVNode->NestedList, "ReferenceInformationType");
			const FBXNode* UVNode = file.searchNodeRecursive(LayerElementUVNode->NestedList, "UV");

			DEBUG_ASSERT(MappingInformationTypeNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::S);
			DEBUG_ASSERT(ReferenceInformationTypeNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::S);
			DEBUG_ASSERT(UVNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::d);

			const ref<FBXSpecialProperty> MappingInformationTypeProperty = std::static_pointer_cast<FBXSpecialProperty>(MappingInformationTypeNode->PropertyList[0]);
			const ref<FBXSpecialProperty> ReferenceInformationTypeProperty = std::static_pointer_cast<FBXSpecialProperty>(ReferenceInformationTypeNode->PropertyList[0]);
			const ref<FBXArrayProperty<float64>> UVProperty = std::static_pointer_cast<FBXArrayProperty<float64>>(UVNode->PropertyList[0]);

			const std::string_view MappingInformationType(pointer_cast<const char8*>(MappingInformationTypeProperty->Data.begin()), MappingInformationTypeProperty->Data.count);
			const std::string_view ReferenceInformationType(pointer_cast<const char8*>(ReferenceInformationTypeProperty->Data.begin()), ReferenceInformationTypeProperty->Data.count);

			if (MappingInformationType == "ByPolygonVertex") {
				if (ReferenceInformationType == "Direct") {
					ENGINE_ERROR_UNIMPLEMENTED();
				}
				else if (ReferenceInformationType == "IndexToDirect") {
					const FBXNode* UVIndexNode = file.searchNodeRecursive(LayerElementUVNode->NestedList, "UVIndex");

					DEBUG_ASSERT(UVIndexNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::i);

					const ref<FBXArrayProperty<sint32>> UVIndexProperty = std::static_pointer_cast<FBXArrayProperty<sint32>>(UVIndexNode->PropertyList[0]);

					vertexTextureCoordinateComponents = UVProperty->Contents;
					vertexTextureCoordinateIndices = sequence<uint32>(vertexPositionIndices.count);

					uint32 vertexOffset = 0;

					for (uint32 polygonOffset = 0; polygonOffset < UVIndexProperty->Contents.count; polygonOffset += 1) {
						for (uint32 triangleOffset = 0; triangleOffset < UVIndexProperty->Contents.count - polygonOffset; triangleOffset += 1) {
							const sint32 vertexN2Index = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 2];

							vertexTextureCoordinateIndices[vertexOffset] = UVIndexProperty->Contents[polygonOffset];
							vertexTextureCoordinateIndices[vertexOffset] = UVIndexProperty->Contents[polygonOffset + triangleOffset + 1];
							vertexTextureCoordinateIndices[vertexOffset] = UVIndexProperty->Contents[polygonOffset + triangleOffset + 2];

							vertexOffset += 3;

							if (vertexN2Index < 0) {
								polygonOffset += triangleOffset + 2;

								break;
							}
						}
					}
				}
				else {
					ENGINE_ERROR("Encountered unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);
					ABORT();

					return;
				}
			}
			else if (MappingInformationType == "ByVertex" || MappingInformationType == "ByVertice") {
				vertexTextureCoordinateIndices = sequence<uint32>(vertexPositionIndices);

				if (ReferenceInformationType == "Direct") {
					vertexTextureCoordinateComponents = UVProperty->Contents;
				}
				else if (ReferenceInformationType == "IndexToDirect") {
					ENGINE_ERROR_UNIMPLEMENTED();
				}
				else if (ReferenceInformationType == "ByVertex") {
					ENGINE_ERROR_UNIMPLEMENTED();
				}
				else {
					ENGINE_ERROR("Encountered unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);
					ABORT();

					return;
				}
			}
			else {
				ENGINE_ERROR("Encountered unhandled MappingInformationType value \"[]\".", MappingInformationType);
				ABORT();

				return;
			}
		}

		ENGINE_ASSERT(vertexTextureCoordinateIndices.count == vertexPositionIndices.count, "Vertex texture coordinate index count is inconsistent with vertex position index count.");

		const uint32 geometryAssetIdentifier = AssetLibrary::createAssetIdentifier();
		const uint32 clusterCount = static_cast<uint32>(ceil(static_cast<float32>(indexCount) / CLUSTER_INDEX_COUNT));
		const uint32 vertexPositionCount = static_cast<uint32>(vertexPositionComponents.count) / Geometry3D::VERTEX_POSITION_STRIDE;
		const uint32 vertexNormalCount = static_cast<uint32>(vertexNormalComponents.count) / Geometry3D::VERTEX_NORMAL_STRIDE;
		const uint32 vertexTextureCoordinateCount = static_cast<uint32>(vertexTextureCoordinateComponents.count) / Geometry3D::VERTEX_TEXTURE_COORDINATE_STRIDE;

		const uint32 largestVertexPositionIndex = vertexPositionCount - 1;
		const uint32 largestVertexNormalIndex = vertexNormalCount - 1;
		const uint32 largestVertexTextureCoordinateIndex = vertexTextureCoordinateCount - 1;

		const uint8 vertexPositionIndexBitCount = calculateBitCount(largestVertexPositionIndex);
		const uint8 vertexNormalIndexBitCount = calculateBitCount(largestVertexNormalIndex);
		const uint8 vertexTextureCoordinateIndexBitCount = calculateBitCount(largestVertexTextureCoordinateIndex);

		const uint64 totalIndexByteCount = static_cast<uint64>(ceil(indexCount * (vertexPositionIndexBitCount + vertexNormalIndexBitCount + vertexTextureCoordinateIndexBitCount) / 8.0f));

		const uint64 outputSize =
			Asset::HEADER_SIZE +
			5 * sizeof(uint32) +
			vertexPositionComponents.count * sizeof(float32) +
			vertexNormalComponents.count * sizeof(float32) +
			vertexTextureCoordinateComponents.count * sizeof(float32) +
			totalIndexByteCount;

		ByteStream<StreamOperation::WRITE> byteStream(outputSize, Endianness::LITTLE_ENDIAN);
		byteStream.write<AssetSignature>(AssetSignature::TGE);
		byteStream.write<uint32>(geometryAssetIdentifier);
		byteStream.write<VersionNumber>({0, 0, 0});
		byteStream.write<uint32>(clusterCount);
		byteStream.write<uint32>(vertexPositionCount);

		for (const float64 vertexPositionComponent : vertexPositionComponents) {
			byteStream.write<float32>(static_cast<float32>(vertexPositionComponent));
		}

		byteStream.write<uint32>(vertexNormalCount);

		for (const float64 vertexNormalComponent : vertexNormalComponents) {
			byteStream.write<float32>(static_cast<float32>(vertexNormalComponent));
		}

		byteStream.write<uint32>(vertexTextureCoordinateCount);

		for (const float64 vertexTextureCoordinateComponent : vertexTextureCoordinateComponents) {
			byteStream.write<float32>(static_cast<float32>(vertexTextureCoordinateComponent));
		}

		byteStream.write<uint32>(indexCount);

		BitStream<StreamOperation::WRITE> bitStream(byteStream.getBytePosition(), byteStream.getRemainingSize(), Endianness::LITTLE_ENDIAN);

		for (const uint32 vertexPositionIndex : vertexPositionIndices) {
			bitStream.write<uint32>(vertexPositionIndex, vertexPositionIndexBitCount);
		}

		for (const uint32 vertexNormalIndex : vertexNormalIndices) {
			bitStream.write<uint32>(vertexNormalIndex, vertexNormalIndexBitCount);
		}

		for (const uint32 vertexTextureCoordinateIndex : vertexTextureCoordinateIndices) {
			bitStream.write<uint32>(vertexTextureCoordinateIndex, vertexTextureCoordinateIndexBitCount);
		}

		const std::string outputPath = assetOutputPath + "Geometry/" + directoryEntry.path().stem().string() + ".tge";

		Writer writer = Filesystem::openWriter(outputPath);
		writer.write(byteStream.data, byteStream.size);

		ENGINE_INFO("Generated geometry asset (ID: []).", geometryAssetIdentifier);
	}
}