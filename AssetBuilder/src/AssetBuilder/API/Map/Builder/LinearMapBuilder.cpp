// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Map/Builder/LinearMapBuilder.hpp"

namespace Trident {
	LinearMapBuilder::LinearMapBuilder(const MapDescriptor& descriptor) :
		MapBuilder(descriptor)
	{}

	uint32 LinearMapBuilder::createRectangularRoom(const RectangularRoomDescriptor& descriptor) {
		Room room{};
		room.faceIndices = {faceCount + 0, faceCount + 1, faceCount + 2, faceCount + 3, faceCount + 4, faceCount + 5};

		const svec3<float32> origin = descriptor.origin;
		const float32 halfWidth = descriptor.size.x * 0.5f;
		const float32 height = descriptor.size.y;
		const float32 halfDepth = descriptor.size.z * 0.5f;

		const FaceVertex vertices[] = {
			{origin + svec3<float32>{-halfWidth,  height,  halfDepth}, {0.0f, 0.0f}},
			{origin + svec3<float32>{ halfWidth,  height,  halfDepth}, {0.0f, 0.0f}},
			{origin + svec3<float32>{ halfWidth,  height, -halfDepth}, {0.0f, 0.0f}},
			{origin + svec3<float32>{-halfWidth,  height, -halfDepth}, {0.0f, 0.0f}},
		};

		Face& floorFace = createFace(vertices, {0.0f, 1.0f, 0.0f});

		Face ceilingFace{};
		ceilingFace.vertexCount = 4;
		ceilingFace.indexRange.offset = faceIndexCount;
		ceilingFace.normal = {0.0f, -1.0f, 0.0f};

		indices[faceIndexCount + 0] = indices[floorFace.indexRange.offset + 3];
		indices[faceIndexCount + 1] = indices[floorFace.indexRange.offset + 2];
		indices[faceIndexCount + 2] = indices[floorFace.indexRange.offset + 1];
		indices[faceIndexCount + 3] = indices[floorFace.indexRange.offset + 0];

		faces[faceCount] = ceilingFace;

		faceIndexCount += 4;
		vertexCount += 4;
		triangleCount += 2;
		faceCount += 1;

		extrudeFace(floorFace, -height);

		rooms.emplace_back(room);

		return static_cast<uint32>(rooms.size() - 1);
	}
}