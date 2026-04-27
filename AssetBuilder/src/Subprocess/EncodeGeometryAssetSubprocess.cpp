// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Decoder/Decoder.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include "../File/FBX.hpp"
#include "EncodeGeometryAssetSubprocess.hpp"
#include "TriangleTangents.hpp"

namespace Trident {
	Geometry3DData DecodeFBXGeometry(view<uint8> inputData, uint32 identifier) {
		const File<FileFormat::FBX> file = Decoder<FileFormat::FBX>::decode(inputData);

		// TODO: Ensure that FBX nodes exist before using them.
		const FBXNode* ObjectsNode = file.searchNodeRecursive(file.NodeList, "Objects");
		const FBXNode* GeometryNode = file.searchNodeRecursive(ObjectsNode->NestedList, "Geometry");
		const FBXNode* VerticesNode = file.searchNodeRecursive(GeometryNode->NestedList, "Vertices");
		const FBXNode* PolygonVertexIndexNode = file.searchNodeRecursive(GeometryNode->NestedList, "PolygonVertexIndex");

		DEBUG_ASSERT(VerticesNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::d);
		DEBUG_ASSERT(PolygonVertexIndexNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::i);

		const ref<FBXArrayProperty<float64>> VerticesProperty = std::static_pointer_cast<FBXArrayProperty<float64>>(VerticesNode->PropertyList[0]);
		const ref<FBXArrayProperty<sint32>> PolygonVertexIndexProperty = std::static_pointer_cast<FBXArrayProperty<sint32>>(PolygonVertexIndexNode->PropertyList[0]);

		const sequence<float64>& vertexPositionComponents = VerticesProperty->Contents;

		ENGINE_ASSERT(vertexPositionComponents.count() >= 3, "Invalid vertex position component buffer length.");
		ENGINE_ASSERT(PolygonVertexIndexProperty->Contents.count() >= 3, "Invalid vertex position index buffer length.");

		uint32 indexCount = 0;

		// Get vertex position index buffer size.
		{
			uint32 polygonOffset = 0;
			uint32 triangleOffset = 0;

			while (polygonOffset < PolygonVertexIndexProperty->Contents.count()) {
				while (triangleOffset < PolygonVertexIndexProperty->Contents.count() - polygonOffset) {
					const sint32 thirdIndex = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 2];

					indexCount += 3;

					if (thirdIndex < 0) {
						// A negative index means it's the last index in the current polygon.
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
		uint32 polygonOffset = 0;
		uint32 indexOffset = 0;

		// Parse vertex position indices.
		while (polygonOffset < PolygonVertexIndexProperty->Contents.count()) {
			uint32 triangleOffset = 0;

			while (triangleOffset < PolygonVertexIndexProperty->Contents.count() - polygonOffset) {
				const sint32 indexA = PolygonVertexIndexProperty->Contents[polygonOffset];
				const sint32 indexB = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 1];
				const sint32 indexC = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 2];

				vertexPositionIndices[indexOffset + 0] = indexA;
				vertexPositionIndices[indexOffset + 1] = indexB;

				indexOffset += 3;

				if (indexC < 0) {
					vertexPositionIndices[indexOffset - 1] = ~indexC;

					polygonOffset += triangleOffset + 2;

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

			const view<char8> MappingInformationType(pointer_cast<const char8*>(MappingInformationTypeProperty->Data.begin()), MappingInformationTypeProperty->Data.count());
			const view<char8> ReferenceInformationType(pointer_cast<const char8*>(ReferenceInformationTypeProperty->Data.begin()), ReferenceInformationTypeProperty->Data.count());

			if (MappingInformationType == "ByPolygonVertex") {
				if (ReferenceInformationType == "Direct") {
					vertexNormalComponents = sequence<float64>(vertexPositionComponents.count());
					vertexNormalIndices = vertexPositionIndices;

					// Sum normals for each individual vertex.
					for (uint32 vertexNormalIndexOffset = 0; vertexNormalIndexOffset < PolygonVertexIndexProperty->Contents.count(); vertexNormalIndexOffset += 1) {
						const uint32 vertexNormalIndex = PolygonVertexIndexProperty->Contents[vertexNormalIndexOffset] < 0 ? ~PolygonVertexIndexProperty->Contents[vertexNormalIndexOffset] : PolygonVertexIndexProperty->Contents[vertexNormalIndexOffset];

						vertexNormalComponents[vertexNormalIndex * 3 + 0] += NormalsProperty->Contents[vertexNormalIndexOffset * 3 + 0];
						vertexNormalComponents[vertexNormalIndex * 3 + 1] += NormalsProperty->Contents[vertexNormalIndexOffset * 3 + 1];
						vertexNormalComponents[vertexNormalIndex * 3 + 2] += NormalsProperty->Contents[vertexNormalIndexOffset * 3 + 2];
					}

					// Normalize the sum and overwrite.
					for (uint32 vertexNormalIndexOffset = 0; vertexNormalIndexOffset < vertexNormalComponents.count(); vertexNormalIndexOffset += 3) {
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
				else if (ReferenceInformationType == "IndexToDirect") {
					const FBXNode* NormalsIndexNode = file.searchNodeRecursive(LayerElementNormalNode->NestedList, "NormalsIndex");

					DEBUG_ASSERT(NormalsIndexNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::i);

					const ref<FBXArrayProperty<sint32>> NormalsIndexProperty = std::static_pointer_cast<FBXArrayProperty<sint32>>(NormalsIndexNode->PropertyList[0]);

					vertexNormalComponents = NormalsProperty->Contents;
					vertexNormalIndices = sequence<uint32>(vertexPositionIndices.count());

					uint32 polygonOffset = 0;
					uint32 indexOffset = 0;

					// TODO: This section is similar to the index count section above; maybe try merging the two.
					while (polygonOffset < NormalsIndexProperty->Contents.count()) {
						uint32 triangleOffset = 0;

						while (triangleOffset < NormalsIndexProperty->Contents.count() - polygonOffset) {
							const sint32 indexC = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 2];

							vertexNormalIndices[indexOffset + 0] = NormalsIndexProperty->Contents[polygonOffset];
							vertexNormalIndices[indexOffset + 1] = NormalsIndexProperty->Contents[polygonOffset + triangleOffset + 1];
							vertexNormalIndices[indexOffset + 2] = NormalsIndexProperty->Contents[polygonOffset + triangleOffset + 2];

							indexOffset += 3;

							if (indexC < 0) {
								polygonOffset += triangleOffset + 2;

								break;
							}

							triangleOffset += 1;
						}

						polygonOffset += 1;
					}
				}
				else {
					ENGINE_ERROR("Encountered unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);
					ABORT();

					return {};
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

					vertexNormalComponents = sequence<float64>(NormalsProperty->Contents.count());

					for (uint32 index = 0; index < NormalsIndexProperty->Contents.count(); index += 1) {
						const sint32 normalIndex = NormalsIndexProperty->Contents[index];

						ENGINE_ASSERT(normalIndex >= 0, "Invalid normal index.");

						// TODO: Use std::copy here?
						for (uint32 componentIndex = 0; componentIndex < 3; componentIndex += 1) {
							vertexNormalComponents[index * 3 + componentIndex] = NormalsProperty->Contents[normalIndex * 3 + componentIndex];
						}
					}
				}
				else {
					ENGINE_ERROR("Encountered unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);
					ABORT();

					return {};
				}
			}
			else {
				ENGINE_ERROR("Encountered unhandled MappingInformationType value \"[]\".", MappingInformationType);
				ABORT();

				return {};
			}
		}

		ENGINE_ASSERT(vertexNormalIndices.count() == vertexPositionIndices.count(), "Vertex normal index count is inconsistent with vertex position index count.");

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

			const view<char8> MappingInformationType(pointer_cast<const char8*>(MappingInformationTypeProperty->Data.begin()), MappingInformationTypeProperty->Data.count());
			const view<char8> ReferenceInformationType(pointer_cast<const char8*>(ReferenceInformationTypeProperty->Data.begin()), ReferenceInformationTypeProperty->Data.count());

			if (MappingInformationType == "ByPolygonVertex") {
				if (ReferenceInformationType == "Direct") {
					ENGINE_ERROR_UNIMPLEMENTED();
				}
				else if (ReferenceInformationType == "IndexToDirect") {
					const FBXNode* UVIndexNode = file.searchNodeRecursive(LayerElementUVNode->NestedList, "UVIndex");

					DEBUG_ASSERT(UVIndexNode->PropertyList[0]->TypeCode == FBXPropertyTypeCode::i);

					const ref<FBXArrayProperty<sint32>> UVIndexProperty = std::static_pointer_cast<FBXArrayProperty<sint32>>(UVIndexNode->PropertyList[0]);

					vertexTextureCoordinateComponents = UVProperty->Contents;
					vertexTextureCoordinateIndices = sequence<uint32>(vertexPositionIndices.count());

					uint32 polygonOffset = 0;
					uint32 indexOffset = 0;

					// TODO: This section is similar to the index count section above; maybe try merging the two.
					while (polygonOffset < UVIndexProperty->Contents.count()) {
						uint32 triangleOffset = 0;

						while (triangleOffset < UVIndexProperty->Contents.count() - polygonOffset) {
							const sint32 indexC = PolygonVertexIndexProperty->Contents[polygonOffset + triangleOffset + 2];

							vertexTextureCoordinateIndices[indexOffset + 0] = UVIndexProperty->Contents[polygonOffset];
							vertexTextureCoordinateIndices[indexOffset + 1] = UVIndexProperty->Contents[polygonOffset + triangleOffset + 1];
							vertexTextureCoordinateIndices[indexOffset + 2] = UVIndexProperty->Contents[polygonOffset + triangleOffset + 2];

							indexOffset += 3;

							if (indexC < 0) {
								polygonOffset += triangleOffset + 2;

								break;
							}

							triangleOffset += 1;
						}

						polygonOffset += 1;
					}
				}
				else {
					ENGINE_ERROR("Encountered unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);
					ABORT();

					return {};
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

					return {};
				}
			}
			else {
				ENGINE_ERROR("Encountered unhandled MappingInformationType value \"[]\".", MappingInformationType);
				ABORT();

				return {};
			}
		}

		ENGINE_ASSERT(vertexTextureCoordinateIndices.count() == vertexPositionIndices.count(), "Vertex texture coordinate index count is inconsistent with vertex position index count.");

		sequence<float32> finalVertexPositionComponents(vertexPositionComponents.count());
		sequence<float32> finalVertexNormalComponents(vertexNormalComponents.count());
		sequence<float32> finalVertexTextureCoordinateComponents(vertexTextureCoordinateComponents.count());
		index componentOffset = 0;

		while (componentOffset < finalVertexPositionComponents.count()) {
			finalVertexPositionComponents[componentOffset + 0] = static_cast<float32>(vertexPositionComponents[componentOffset + 0]);
			finalVertexPositionComponents[componentOffset + 1] = static_cast<float32>(vertexPositionComponents[componentOffset + 1]);
			finalVertexPositionComponents[componentOffset + 2] = static_cast<float32>(vertexPositionComponents[componentOffset + 2]);

			componentOffset += 3;
		}

		componentOffset = 0;

		while (componentOffset < finalVertexNormalComponents.count()) {
			finalVertexNormalComponents[componentOffset + 0] = static_cast<float32>(vertexNormalComponents[componentOffset + 0]);
			finalVertexNormalComponents[componentOffset + 1] = static_cast<float32>(vertexNormalComponents[componentOffset + 1]);
			finalVertexNormalComponents[componentOffset + 2] = static_cast<float32>(vertexNormalComponents[componentOffset + 2]);

			componentOffset += 3;
		}

		componentOffset = 0;

		while (componentOffset < finalVertexTextureCoordinateComponents.count()) {
			finalVertexTextureCoordinateComponents[componentOffset + 0] = static_cast<float32>(vertexTextureCoordinateComponents[componentOffset + 0]);
			finalVertexTextureCoordinateComponents[componentOffset + 1] = static_cast<float32>(vertexTextureCoordinateComponents[componentOffset + 1]);

			componentOffset += 2;
		}

		const sequence<float32> triangleTangentComponents = calculateTriangleTangentComponents(finalVertexPositionComponents, finalVertexTextureCoordinateComponents, vertexPositionIndices, vertexTextureCoordinateIndices);

		return {
			.vertexPositionComponents = finalVertexPositionComponents,
			.vertexNormalComponents = finalVertexNormalComponents,
			.vertexTextureCoordinateComponents = finalVertexTextureCoordinateComponents,
			.triangleTangentComponents = triangleTangentComponents,
			.vertexPositionIndices = vertexPositionIndices,
			.vertexNormalIndices = vertexNormalIndices,
			.vertexTextureCoordinateIndices = vertexTextureCoordinateIndices,
			.flags = {},
			.hullLineIndices = {},
			.hullEdges = {},
			.hullFaceEdgeIndices = {},
			.hullFaces = {},
			.hull = {},
		};
	}
}