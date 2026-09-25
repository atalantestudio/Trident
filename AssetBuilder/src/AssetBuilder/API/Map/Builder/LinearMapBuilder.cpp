// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Map/Builder/LinearMapBuilder.hpp"

namespace Trident {
	uint32 LinearMapBuilder::createRectangularRoom(const RectangularRoomDescriptor& descriptor) {
		Room room{};
		room.faceIndices = {
			staticGeometryFaceCount + 0,
			staticGeometryFaceCount + 1,
			staticGeometryFaceCount + 2,
			staticGeometryFaceCount + 3,
			staticGeometryFaceCount + 4,
			staticGeometryFaceCount + 5,
		};

		const svec3<float32> origin = descriptor.origin;
		const float32 halfWidth = descriptor.size.x * 0.5f;
		const float32 height = descriptor.size.y;
		const float32 halfDepth = descriptor.size.z * 0.5f;

		const svec3<float32> vertices[] = {
			origin + svec3<float32>{-halfWidth,  height,  halfDepth},
			origin + svec3<float32>{ halfWidth,  height,  halfDepth},
			origin + svec3<float32>{ halfWidth,  height, -halfDepth},
			origin + svec3<float32>{-halfWidth,  height, -halfDepth},
		};

		Face& floorFace = createFace({
			.vertices = vertices,
			.normal = {0.0f, 1.0f, 0.0f},
			.material = descriptor.material,
			.lightmap = descriptor.lightmap,
		});

		Face ceilingFace{};
		ceilingFace.vertexCount = 4;
		ceilingFace.indexRange.offset = staticGeometryFaceVertexCount;
		ceilingFace.normal = {0.0f, -1.0f, 0.0f};
		ceilingFace.triangleOffset = staticGeometryFaceTriangleCount;
		ceilingFace.geometryIndex = floorFace.geometryIndex;
		ceilingFace.material = descriptor.material;
		ceilingFace.lightmap = descriptor.lightmap;
		ceilingFace.boundingBox.min = {max<float32>(), max<float32>()};
		ceilingFace.boundingBox.max = {min<float32>(), min<float32>()};

		indices[staticGeometryFaceVertexCount + 0] = indices[floorFace.indexRange.offset + 3];
		indices[staticGeometryFaceVertexCount + 1] = indices[floorFace.indexRange.offset + 2];
		indices[staticGeometryFaceVertexCount + 2] = indices[floorFace.indexRange.offset + 1];
		indices[staticGeometryFaceVertexCount + 3] = indices[floorFace.indexRange.offset + 0];

		faces[staticGeometryFaceCount] = ceilingFace;

		IntermediateGeometry& geometry = geometries[floorFace.geometryIndex];
		geometry.faceIndices.emplace_back(staticGeometryFaceCount);

		staticGeometryFaceVertexCount += 4;
		staticGeometryFaceTriangleCount += 2;
		staticGeometryFaceCount += 1;
		lightmapCoordinateCount += 4;
		lightmapCount += 1;

		extrudeFace(floorFace, -height);

		rooms.emplace_back(room);

		return static_cast<uint32>(rooms.size() - 1);
	}
}