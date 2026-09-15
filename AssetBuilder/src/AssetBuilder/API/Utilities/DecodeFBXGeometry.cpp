// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Filesystem.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "../../../File/FBX.hpp"

namespace Trident {
	static sint32 parseCoordAxisSign(const File<FileFormat::FBX>& file) {
		const FBX::Node* GlobalSettings = FBX::findNode(file, "GlobalSettings");
		const FBX::Node* Properties70 = FBX::findNode(file, GlobalSettings, "Properties70");
		const sint32 UpAxis = FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "UpAxis");
		const sint32 FrontAxis = FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "FrontAxis");
		const sint32 CoordAxis = FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "CoordAxis");

		if (UpAxis != 1 || FrontAxis != 2 || CoordAxis != 0) {
			ENGINE_WARNING("Unsupported axis order (UpAxis = [], FrontAxis = [], CoordAxis = []). Using left-handed coordinate system with +Y up.", UpAxis, FrontAxis, CoordAxis);
		}

		return FBX::resolvePrimitiveProperty<sint32>(file, Properties70, "CoordAxisSign");
	}

	static void parseTransform(const File<FileFormat::FBX>& file, const FBX::Node* Objects, quat<float32>& orientation, svec3<float32>& scaling) {
		const FBX::Node* Model = FBX::findNode(file, Objects, "Model");
		const FBX::Node* Properties70 = FBX::findNode(file, Model, "Properties70");
		const svec3<float64> Lcl_Rotation = FBX::resolveVec3Property<float64>(file, Properties70, "Lcl Rotation");

		orientation = normalized(
			quat<float32>::axisAngle(UP, radians(static_cast<float32>(Lcl_Rotation.y))) *
			quat<float32>::axisAngle(RIGHT, radians(static_cast<float32>(Lcl_Rotation.x))) *
			quat<float32>::axisAngle(FORWARD, radians(static_cast<float32>(Lcl_Rotation.z)))
		);

		svec3<float64> Lcl_Scaling = FBX::resolveVec3Property<float64>(file, Properties70, "Lcl Scaling");

		if (
			abs(Lcl_Scaling.x - Lcl_Scaling.y) >= 0.001 ||
			abs(Lcl_Scaling.x - Lcl_Scaling.z) >= 0.001
		) {
			ENGINE_WARNING("Non-uniform scaling is not supported for the Lcl Scaling property. Broadcasting X scaling to Y and Z.");

			Lcl_Scaling.y = Lcl_Scaling.x;
			Lcl_Scaling.z = Lcl_Scaling.x;
		}

		scaling = {
			static_cast<float32>(Lcl_Scaling.x),
			static_cast<float32>(Lcl_Scaling.y),
			static_cast<float32>(Lcl_Scaling.z),
		};
	}

	static sequence<uint32> parseVertexPositionIndices(ref<FBX::ArrayProperty<sint32>> PolygonVertexIndexProperty, sint32 CoordAxisSign) {
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

		return vertexPositionIndices;
	}

	static void parseVertexNormals(
		sequence<float64>& vertexNormalComponents,
		sequence<uint32>& vertexNormalIndices,
		const sequence<uint32>& vertexPositionIndices,
		const File<FileFormat::FBX>& file,
		const FBX::Node* GeometryNode,
		ref<FBX::ArrayProperty<float64>> VerticesProperty,
		ref<FBX::ArrayProperty<sint32>> PolygonVertexIndexProperty
	) {
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

				return;
			}

			if (ReferenceInformationType == "IndexToDirect") {
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

				return;
			}

			ENGINE_ASSERT(false, "Unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);

			return;
		}

		if (MappingInformationType == "ByVertex" || MappingInformationType == "ByVertice") {
			vertexNormalIndices = vertexPositionIndices;

			if (ReferenceInformationType == "Direct") {
				vertexNormalComponents = NormalsProperty->contents;

				return;
			}

			if (ReferenceInformationType == "IndexToDirect") {
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

				return;
			}

			ENGINE_ASSERT(false, "Unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);

			return;
		}

		ENGINE_ASSERT(false, "Unhandled MappingInformationType value \"[]\".", MappingInformationType);
	}

	static void parseVertexTextureCoordinates(
		sequence<float64>& vertexTextureCoordinateComponents,
		sequence<uint32>& vertexTextureCoordinateIndices,
		const sequence<uint32>& vertexPositionIndices,
		const File<FileFormat::FBX>& file,
		const FBX::Node* GeometryNode,
		ref<FBX::ArrayProperty<sint32>> PolygonVertexIndexProperty,
		sint32 CoordAxisSign
	) {
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

				return;
			}

			if (ReferenceInformationType == "IndexToDirect") {
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

				return;
			}

			ENGINE_ASSERT(false, "Unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);

			return;
		}

		if (MappingInformationType == "ByVertex" || MappingInformationType == "ByVertice") {
			vertexTextureCoordinateIndices = vertexPositionIndices;

			if (ReferenceInformationType == "Direct") {
				vertexTextureCoordinateComponents = UVProperty->contents;

				return;
			}

			if (ReferenceInformationType == "IndexToDirect") {
				ENGINE_ERROR_UNIMPLEMENTED();

				return;
			}

			if (ReferenceInformationType == "ByVertex") {
				ENGINE_ERROR_UNIMPLEMENTED();

				return;
			}

			ENGINE_ASSERT(false, "Unhandled ReferenceInformationType value \"[]\".", ReferenceInformationType);

			return;
		}

		ENGINE_ASSERT(false, "Unhandled MappingInformationType value \"[]\".", MappingInformationType);
	}

	BuildGeometry3D decodeFBXGeometry(view<uint8> inputData, uint32 identifier) {
		const File<FileFormat::FBX> file = Decoder<FileFormat::FBX>::decode(inputData);
		const sint32 CoordAxisSign = parseCoordAxisSign(file);
		const FBX::Node* ObjectsNode = FBX::findNode(file, "Objects");
		const FBX::Node* GeometryNode = FBX::findNode(file, ObjectsNode, "Geometry");
		const FBX::Node* VerticesNode = FBX::findNode(file, GeometryNode, "Vertices");

		DEBUG_ASSERT(file.properties[VerticesNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::d);

		const FBX::Node* PolygonVertexIndexNode = FBX::findNode(file, GeometryNode, "PolygonVertexIndex");

		DEBUG_ASSERT(file.properties[PolygonVertexIndexNode->propertyRange.offset]->typeCode == FBX::PropertyTypeCode::i);

		const ref<FBX::ArrayProperty<float64>> VerticesProperty = ref_cast<FBX::ArrayProperty<float64>>(file.properties[VerticesNode->propertyRange.offset]);

		ENGINE_ASSERT(VerticesProperty->contents.count() >= 3, "Invalid vertex position component buffer length.");

		quat<float32> orientation;
		svec3<float32> scaling;

		parseTransform(file, ObjectsNode, orientation, scaling);

		const mat4x4<float32> transform = (
			mat4x4<float32>::scaling(scaling) *
			mat4x4<float32>(orientation)
		);

		sequence<svec3<float32>> positions(VerticesProperty->contents.count() / 3);
		uint32 positionIndex = 0;

		while (positionIndex < positions.count()) {
			positions[positionIndex] = {
				static_cast<float32>(VerticesProperty->contents[positionIndex * 3 + 0]),
				static_cast<float32>(VerticesProperty->contents[positionIndex * 3 + 1]),
				static_cast<float32>(VerticesProperty->contents[positionIndex * 3 + 2]),
			};

			if (CoordAxisSign == 1) {
				positions[positionIndex] *= -1.0f;
			}

			// TODO(lyah/scalar)
			positions[positionIndex] = svec3<float32>(multiplyTransform(vec3<float32>(positions[positionIndex]), transform));

			positionIndex += 1;
		}

		const ref<FBX::ArrayProperty<sint32>> PolygonVertexIndexProperty = ref_cast<FBX::ArrayProperty<sint32>>(file.properties[PolygonVertexIndexNode->propertyRange.offset]);

		ENGINE_ASSERT(PolygonVertexIndexProperty->contents.count() >= 3, "Invalid vertex position index buffer length.");

		const sequence<uint32> vertexPositionIndices = parseVertexPositionIndices(PolygonVertexIndexProperty, CoordAxisSign);

		sequence<float64> vertexNormalComponents;
		sequence<uint32> vertexNormalIndices;
		sequence<float64> vertexTextureCoordinateComponents;
		sequence<uint32> vertexTextureCoordinateIndices;

		parseVertexNormals(vertexNormalComponents, vertexNormalIndices, vertexPositionIndices, file, GeometryNode, VerticesProperty, PolygonVertexIndexProperty);
		parseVertexTextureCoordinates(vertexTextureCoordinateComponents, vertexTextureCoordinateIndices, vertexPositionIndices, file, GeometryNode, PolygonVertexIndexProperty, CoordAxisSign);

		sequence<svec3<float32>> normals(vertexNormalComponents.count() / 3);
		sequence<svec2<float32>> textureCoordinates(vertexTextureCoordinateComponents.count() / 2);
		uint32 index = 0;

		while (index < normals.count()) {
			normals[index] = {
				static_cast<float32>(vertexNormalComponents[index * 3 + 0]),
				static_cast<float32>(vertexNormalComponents[index * 3 + 1]),
				static_cast<float32>(vertexNormalComponents[index * 3 + 2]),
			};

			normals[index] = svec3<float32>(vec3<float32>(normals[index]) * orientation);

			index += 1;
		}

		index = 0;

		while (index < textureCoordinates.count()) {
			textureCoordinates[index] = {
				static_cast<float32>(vertexTextureCoordinateComponents[index * 2 + 0]),
				1.0f - static_cast<float32>(vertexTextureCoordinateComponents[index * 2 + 1]),
			};

			index += 1;
		}

		return inflateFBXStreams(positions, normals, textureCoordinates, vertexPositionIndices, vertexNormalIndices, vertexTextureCoordinateIndices);
	}
}