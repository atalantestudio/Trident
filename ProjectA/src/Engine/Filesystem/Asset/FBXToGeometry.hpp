#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/Filesystem/File/FBXFile.hpp"

namespace ProjectA {
	static ref<Geometry3D> FBXToGeometry3D(const File<FileFormat::FBX>& file) {
		const FBXNode* ObjectsNode = file.searchNodeRecursive(file.NodeList, "Objects");
		const FBXNode* PolygonVertexIndexNode = file.searchNodeRecursive(ObjectsNode->NestedList, "PolygonVertexIndex");

		// Assume vertex position indices are stored in an array property.
		const sequence<sint32>& vertexPositionIndices = std::static_pointer_cast<FBXArrayProperty<sint32>>(PolygonVertexIndexNode->PropertyList[0])->Contents;

		ENGINE_ASSERT(vertexPositionIndices.count >= 3, "Invalid vertex position index buffer length.");

		const FBXNode* VerticesNode = file.searchNodeRecursive(ObjectsNode->NestedList, "Vertices");

		// Assume vertex positions are stored in an array property.
		const sequence<float64>& vertexPositionComponents = std::static_pointer_cast<FBXArrayProperty<float64>>(VerticesNode->PropertyList[0])->Contents;

		Geometry3DDescriptor geometryDescriptor;
		geometryDescriptor.vertexPositionComponents = sequence<float32>(vertexPositionComponents.count);

		for (uint32 vertexPositionComponentIndex = 0; vertexPositionComponentIndex < geometryDescriptor.vertexPositionComponents.count; vertexPositionComponentIndex += 1) {
			geometryDescriptor.vertexPositionComponents[vertexPositionComponentIndex] = static_cast<float32>(vertexPositionComponents[vertexPositionComponentIndex]);
		}

		uint64 vertexPositionIndexCount = 0;

		// Get vertex position index buffer size.
		{
			uint32 polygonOffset = 0;
			uint32 triangleOffset = 0;

			while (polygonOffset < vertexPositionIndices.count) {
				while (triangleOffset < vertexPositionIndices.count - polygonOffset) {
					const sint32 thirdIndex = vertexPositionIndices[polygonOffset + triangleOffset + 2];

					vertexPositionIndexCount += 3;

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

		// TODO: Use 16-bit indices when possible.
		geometryDescriptor.vertexPositionIndices = sequence<uint32>(vertexPositionIndexCount);

		uint32 polygonCount = 0;

		// Write vertex position indices.
		{
			uint32 polygonOffset = 0;
			uint32 triangleOffset = 0;
			uint32 vertexOffset = 0;

			while (polygonOffset < vertexPositionIndices.count) {
				while (triangleOffset < vertexPositionIndices.count - polygonOffset) {
					const sint32 indexA = vertexPositionIndices[polygonOffset];
					const sint32 indexB = vertexPositionIndices[polygonOffset + triangleOffset + 1];
					const sint32 indexC = vertexPositionIndices[polygonOffset + triangleOffset + 2];

					geometryDescriptor.vertexPositionIndices[vertexOffset + 0] = indexA;
					geometryDescriptor.vertexPositionIndices[vertexOffset + 1] = indexB;
					geometryDescriptor.vertexPositionIndices[vertexOffset + 2] = indexC;

					vertexOffset += 3;

					if (indexC < 0) {
						geometryDescriptor.vertexPositionIndices[vertexOffset - 1] = ~indexC;

						polygonOffset += triangleOffset + 2;
						polygonCount += 1;

						break;
					}

					triangleOffset += 1;
				}

				polygonOffset += 1;
				triangleOffset = 0;
			}
		}

		return createRef<Geometry3D>(geometryDescriptor);
	}
}