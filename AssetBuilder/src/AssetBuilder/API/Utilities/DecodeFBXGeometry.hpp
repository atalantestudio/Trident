// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Filesystem.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "../../../File/FBX.hpp"

namespace Trident {
	BuildGeometry3D decodeFBXGeometry(view<uint8> inputData, uint32 identifier) {
		const File<FileFormat::FBX> file = Decoder<FileFormat::FBX>::decode(inputData);

		const FBX::Node* GlobalSettings = FBX::findNode(file, "GlobalSettings");
		const FBX::Node* Properties70 = FBX::findNode(file, GlobalSettings, "Properties70");
		const sint32 UpAxis = FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "UpAxis");
		const sint32 FrontAxis = FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "FrontAxis");
		const sint32 CoordAxis = FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "CoordAxis");
		const sint32 CoordAxisSign = FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "CoordAxisSign");
		const FBX::Node* ObjectsNode = FBX::findNode(file, "Objects");
		const FBX::Node* GeometryNode = FBX::findNode(file, ObjectsNode, "Geometry");
		const FBX::Node* VerticesNode = FBX::findNode(file, GeometryNode, "Vertices");

		DEBUG_ASSERT(file.properties[VerticesNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::d);

		const FBX::Node* PolygonVertexIndexNode = FBX::findNode(file, GeometryNode, "PolygonVertexIndex");

		DEBUG_ASSERT(file.properties[PolygonVertexIndexNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::i);

		const ref<FBX::ArrayProperty<float64>> VerticesProperty = ref_cast<FBX::ArrayProperty<float64>>(file.properties[VerticesNode->propertyRange.offset]);

		ENGINE_ASSERT(VerticesProperty->contents.count() >= 3, "Invalid vertex position component buffer length.");

		mat4x4<float32> transform = mat4x4<float32>::identity();
		sequence<svec3<float32>> positions(VerticesProperty->contents.count() / 3);
		uint32 positionIndex = 0;

		switch (CoordAxis) {
			case 0: {
				transform[0] = {static_cast<float32>(-CoordAxisSign), 0.0f, 0.0f, 0.0f};

				break;
			}
			case 1: {
				transform[1] = {0.0f, static_cast<float32>(-CoordAxisSign), 0.0f, 0.0f};

				break;
			}
			case 2: {
				transform[2] = {0.0f, 0.0f, static_cast<float32>(-CoordAxisSign), 0.0f};

				break;
			}
		}

		while (positionIndex < positions.count()) {
			positions[positionIndex] = {
				static_cast<float32>(VerticesProperty->contents[positionIndex * 3 + 0]),
				static_cast<float32>(VerticesProperty->contents[positionIndex * 3 + 1]),
				static_cast<float32>(VerticesProperty->contents[positionIndex * 3 + 2]),
			};

			// TODO(lyah/scalar)
			positions[positionIndex] = svec3<float32>(multiplyTransform(vec3<float32>(positions[positionIndex]), transform));

			positionIndex += 1;
		}

		const ref<FBX::ArrayProperty<sint32>> PolygonVertexIndexProperty = ref_cast<FBX::ArrayProperty<sint32>>(file.properties[PolygonVertexIndexNode->propertyRange.offset]);

		ENGINE_ASSERT(PolygonVertexIndexProperty->contents.count() >= 3, "Invalid vertex position index buffer length.");

		uint32 indexCount = 0;

		// Get vertex position index buffer size.
		{
			uint32 polygonOffset = 0;
			uint32 triangleOffset = 0;

			while (polygonOffset < PolygonVertexIndexProperty->contents.count()) {
				while (triangleOffset < PolygonVertexIndexProperty->contents.count() - polygonOffset) {
					const sint32 thirdIndex = PolygonVertexIndexProperty->contents[polygonOffset + triangleOffset + 2];

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
		while (polygonOffset < PolygonVertexIndexProperty->contents.count()) {
			uint32 triangleOffset = 0;

			while (triangleOffset < PolygonVertexIndexProperty->contents.count() - polygonOffset) {
				const sint32 indexA = PolygonVertexIndexProperty->contents[polygonOffset];
				const sint32 indexB = PolygonVertexIndexProperty->contents[polygonOffset + triangleOffset + 1];
				const sint32 indexC = PolygonVertexIndexProperty->contents[polygonOffset + triangleOffset + 2];

				if (CoordAxisSign == 1) {
					vertexPositionIndices[indexOffset + 0] = indexB;
					vertexPositionIndices[indexOffset + 1] = indexA;
				}
				else {
					vertexPositionIndices[indexOffset + 0] = indexA;
					vertexPositionIndices[indexOffset + 1] = indexB;
				}

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
			const FBX::Node* LayerElementNormalNode = FBX::findNode(file, GeometryNode, "LayerElementNormal");
			const FBX::Node* MappingInformationTypeNode = FBX::findNode(file, LayerElementNormalNode, "MappingInformationType");
			const FBX::Node* ReferenceInformationTypeNode = FBX::findNode(file, LayerElementNormalNode, "ReferenceInformationType");
			const FBX::Node* NormalsNode = FBX::findNode(file, LayerElementNormalNode, "Normals");

			DEBUG_ASSERT(file.properties[MappingInformationTypeNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::S);
			DEBUG_ASSERT(file.properties[ReferenceInformationTypeNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::S);
			DEBUG_ASSERT(file.properties[NormalsNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::d);

			const ref<FBX::SpecialProperty> MappingInformationTypeProperty = ref_cast<FBX::SpecialProperty>(file.properties[MappingInformationTypeNode->propertyRange.offset]);
			const ref<FBX::SpecialProperty> ReferenceInformationTypeProperty = ref_cast<FBX::SpecialProperty>(file.properties[ReferenceInformationTypeNode->propertyRange.offset]);
			const ref<FBX::ArrayProperty<float64>> NormalsProperty = ref_cast<FBX::ArrayProperty<float64>>(file.properties[NormalsNode->propertyRange.offset]);

			const view<char8> MappingInformationType(pointer_cast<const char8*>(MappingInformationTypeProperty->data.begin()), MappingInformationTypeProperty->data.count());
			const view<char8> ReferenceInformationType(pointer_cast<const char8*>(ReferenceInformationTypeProperty->data.begin()), ReferenceInformationTypeProperty->data.count());

			if (MappingInformationType == "ByPolygonVertex") {
				if (ReferenceInformationType == "Direct") {
					vertexNormalComponents = sequence<float64>(VerticesProperty->contents.count());
					vertexNormalIndices = vertexPositionIndices;

					// Sum normals for each individual vertex.
					for (uint32 vertexNormalIndexOffset = 0; vertexNormalIndexOffset < PolygonVertexIndexProperty->contents.count(); vertexNormalIndexOffset += 1) {
						const uint32 vertexNormalIndex = PolygonVertexIndexProperty->contents[vertexNormalIndexOffset] < 0 ? ~PolygonVertexIndexProperty->contents[vertexNormalIndexOffset] : PolygonVertexIndexProperty->contents[vertexNormalIndexOffset];

						vertexNormalComponents[vertexNormalIndex * 3 + 0] += NormalsProperty->contents[vertexNormalIndexOffset * 3 + 0];
						vertexNormalComponents[vertexNormalIndex * 3 + 1] += NormalsProperty->contents[vertexNormalIndexOffset * 3 + 1];
						vertexNormalComponents[vertexNormalIndex * 3 + 2] += NormalsProperty->contents[vertexNormalIndexOffset * 3 + 2];
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
					const FBX::Node* NormalsIndexNode = FBX::findNode(file, LayerElementNormalNode, "NormalsIndex");

					DEBUG_ASSERT(file.properties[NormalsIndexNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::i);

					const ref<FBX::ArrayProperty<sint32>> NormalsIndexProperty = ref_cast<FBX::ArrayProperty<sint32>>(file.properties[NormalsIndexNode->propertyRange.offset]);

					vertexNormalComponents = NormalsProperty->contents;
					vertexNormalIndices = sequence<uint32>(vertexPositionIndices.count());

					uint32 polygonOffset = 0;
					uint32 indexOffset = 0;

					// TODO(asset-builder/geometry/fbx): This section is similar to the index count section above; maybe try merging the two.
					while (polygonOffset < NormalsIndexProperty->contents.count()) {
						uint32 triangleOffset = 0;

						while (triangleOffset < NormalsIndexProperty->contents.count() - polygonOffset) {
							const sint32 indexC = PolygonVertexIndexProperty->contents[polygonOffset + triangleOffset + 2];

							vertexNormalIndices[indexOffset + 0] = NormalsIndexProperty->contents[polygonOffset];
							vertexNormalIndices[indexOffset + 1] = NormalsIndexProperty->contents[polygonOffset + triangleOffset + 1];
							vertexNormalIndices[indexOffset + 2] = NormalsIndexProperty->contents[polygonOffset + triangleOffset + 2];

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
					vertexNormalComponents = NormalsProperty->contents;
				}
				else if (ReferenceInformationType == "IndexToDirect") {
					const FBX::Node* NormalsIndexNode = FBX::findNode(file, LayerElementNormalNode, "NormalsIndex");

					DEBUG_ASSERT(file.properties[NormalsIndexNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::i);

					const ref<FBX::ArrayProperty<sint32>> NormalsIndexProperty = ref_cast<FBX::ArrayProperty<sint32>>(file.properties[NormalsIndexNode->propertyRange.offset]);

					vertexNormalComponents = sequence<float64>(NormalsProperty->contents.count());

					for (uint32 index = 0; index < NormalsIndexProperty->contents.count(); index += 1) {
						const sint32 normalIndex = NormalsIndexProperty->contents[index];

						ENGINE_ASSERT(normalIndex >= 0, "Invalid normal index.");

						// TODO(asset-builder/geometry/fbx): Use std::copy here?
						for (uint32 componentIndex = 0; componentIndex < 3; componentIndex += 1) {
							vertexNormalComponents[index * 3 + componentIndex] = NormalsProperty->contents[normalIndex * 3 + componentIndex];
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
			const FBX::Node* LayerElementUVNode = FBX::findNode(file, GeometryNode, "LayerElementUV");
			const FBX::Node* MappingInformationTypeNode = FBX::findNode(file, LayerElementUVNode, "MappingInformationType");
			const FBX::Node* ReferenceInformationTypeNode = FBX::findNode(file, LayerElementUVNode, "ReferenceInformationType");
			const FBX::Node* UVNode = FBX::findNode(file, LayerElementUVNode, "UV");

			DEBUG_ASSERT(file.properties[MappingInformationTypeNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::S);
			DEBUG_ASSERT(file.properties[ReferenceInformationTypeNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::S);
			DEBUG_ASSERT(file.properties[UVNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::d);

			const ref<FBX::SpecialProperty> MappingInformationTypeProperty = ref_cast<FBX::SpecialProperty>(file.properties[MappingInformationTypeNode->propertyRange.offset]);
			const ref<FBX::SpecialProperty> ReferenceInformationTypeProperty = ref_cast<FBX::SpecialProperty>(file.properties[ReferenceInformationTypeNode->propertyRange.offset]);
			const ref<FBX::ArrayProperty<float64>> UVProperty = ref_cast<FBX::ArrayProperty<float64>>(file.properties[UVNode->propertyRange.offset]);

			const view<char8> MappingInformationType(pointer_cast<const char8*>(MappingInformationTypeProperty->data.begin()), MappingInformationTypeProperty->data.count());
			const view<char8> ReferenceInformationType(pointer_cast<const char8*>(ReferenceInformationTypeProperty->data.begin()), ReferenceInformationTypeProperty->data.count());

			if (MappingInformationType == "ByPolygonVertex") {
				if (ReferenceInformationType == "Direct") {
					ENGINE_ERROR_UNIMPLEMENTED();
				}
				else if (ReferenceInformationType == "IndexToDirect") {
					const FBX::Node* UVIndexNode = FBX::findNode(file, LayerElementUVNode, "UVIndex");

					DEBUG_ASSERT(file.properties[UVIndexNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::i);

					const ref<FBX::ArrayProperty<sint32>> UVIndexProperty = ref_cast<FBX::ArrayProperty<sint32>>(file.properties[UVIndexNode->propertyRange.offset]);

					vertexTextureCoordinateComponents = UVProperty->contents;
					vertexTextureCoordinateIndices = sequence<uint32>(vertexPositionIndices.count());

					uint32 polygonOffset = 0;
					uint32 indexOffset = 0;

					// TODO(asset-builder/geometry/fbx): This section is similar to the index count section above; maybe try merging the two.
					while (polygonOffset < UVIndexProperty->contents.count()) {
						uint32 triangleOffset = 0;

						while (triangleOffset < UVIndexProperty->contents.count() - polygonOffset) {
							const sint32 indexC = PolygonVertexIndexProperty->contents[polygonOffset + triangleOffset + 2];

							vertexTextureCoordinateIndices[indexOffset + 0] = UVIndexProperty->contents[polygonOffset];
							vertexTextureCoordinateIndices[indexOffset + 1] = UVIndexProperty->contents[polygonOffset + triangleOffset + 1];
							vertexTextureCoordinateIndices[indexOffset + 2] = UVIndexProperty->contents[polygonOffset + triangleOffset + 2];

							if (CoordAxisSign == 1) {
								vertexTextureCoordinateIndices[indexOffset + 0] = UVIndexProperty->contents[polygonOffset + triangleOffset + 1];
								vertexTextureCoordinateIndices[indexOffset + 1] = UVIndexProperty->contents[polygonOffset];
							}

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
					vertexTextureCoordinateComponents = UVProperty->contents;
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

		sequence<svec3<float32>> normals(vertexNormalComponents.count() / 3);
		sequence<svec2<float32>> textureCoordinates(vertexTextureCoordinateComponents.count() / 2);

		uint32 normalIndex = 0;

		while (normalIndex < normals.count()) {
			normals[normalIndex] = {
				static_cast<float32>(vertexNormalComponents[normalIndex * 3 + 0]),
				static_cast<float32>(vertexNormalComponents[normalIndex * 3 + 1]),
				static_cast<float32>(vertexNormalComponents[normalIndex * 3 + 2]),
			};

			normalIndex += 1;
		}

		uint32 textureCoordinateIndex = 0;

		while (textureCoordinateIndex < textureCoordinates.count()) {
			textureCoordinates[textureCoordinateIndex] = {
				static_cast<float32>(vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 0]),
				1.0f - static_cast<float32>(vertexTextureCoordinateComponents[textureCoordinateIndex * 2 + 1]),
			};

			textureCoordinateIndex += 1;
		}

		return inflateFBXStreams(positions, normals, textureCoordinates, vertexPositionIndices, vertexNormalIndices, vertexTextureCoordinateIndices);
	}
}