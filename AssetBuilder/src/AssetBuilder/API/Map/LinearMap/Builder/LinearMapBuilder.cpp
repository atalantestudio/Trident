// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Map/LinearMap/Builder/LinearMapBuilder.hpp"

namespace Trident {
	LinearMapBuilder::LinearMapBuilder(const MapDescriptor& descriptor) :
		MapBuilder(descriptor),
		rooms(MAX_ROOM_COUNT)
	{}

	Room& LinearMapBuilder::createRectangularRoom(const RectangularRoomDescriptor& descriptor) {
		Room& room = rooms[roomOffset];
		room.faceIndices = {
			staticGeometryFaceCount + 0,
			staticGeometryFaceCount + 1,
			staticGeometryFaceCount + 2,
			staticGeometryFaceCount + 3,
			staticGeometryFaceCount + 4,
			staticGeometryFaceCount + 5,
		};

		const uint32 facePositionOffset = facePositionCount;
		const vec3<float32> origin = descriptor.origin;
		const float32 halfWidth = descriptor.size.x * 0.5f;
		const float32 height = descriptor.size.y;
		const float32 halfDepth = descriptor.size.z * 0.5f;
		const uint32 floorFaceIndices[] = {
			facePositionOffset + 0,
			facePositionOffset + 1,
			facePositionOffset + 2,
			facePositionOffset + 3,
		};
		const uint32 ceilingFaceIndices[] = {
			facePositionOffset + 3,
			facePositionOffset + 2,
			facePositionOffset + 1,
			facePositionOffset + 0,
		};

		createPoint(origin + vec3<float32>{-halfWidth,  height,  halfDepth});
		createPoint(origin + vec3<float32>{ halfWidth,  height,  halfDepth});
		createPoint(origin + vec3<float32>{ halfWidth,  height, -halfDepth});
		createPoint(origin + vec3<float32>{-halfWidth,  height, -halfDepth});

		Face& floorFace = createFace({
			.indices = floorFaceIndices,
			.normal = {0.0f, 1.0f, 0.0f},
			.material = descriptor.material,
			.textureFiltering = descriptor.textureFiltering,
			.lightmap = descriptor.lightmap,
		});

		createFace({
			.indices = ceilingFaceIndices,
			.normal = {0.0f, -1.0f, 0.0f},
			.material = descriptor.material,
			.textureFiltering = descriptor.textureFiltering,
			.lightmap = descriptor.lightmap,
		});

		extrudeFace(floorFace, floorFace.normal, -height);

		roomOffset += 1;

		return room;
	}

	void LinearMapBuilder::connectFaces(Face& faceA, Face& faceB, view<vec3<float32>> verticesA, view<vec3<float32>> verticesB) {
		DEBUG_ASSERT(faceA.getUniqueIndexCount() == faceB.getUniqueIndexCount());

		// TODO(lyah/scalar)
		DEBUG_ASSERT(abs(dot(faceA.normal, faceB.normal) + 1.0f) < 1e-4f);

		Face& carvingA = carveFace(faceA, verticesA);

		const uint32 carvingAIndex = getLastFaceIndex();
		const float32 distance = dot(verticesB[0] - verticesA[0], faceA.normal);

		extrudeFace(carvingA, carvingA.normal, distance);
		removeFace(carvingAIndex);
		carveFace(faceB, verticesB);
		removeFace(getLastFaceIndex());
	}
}