// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Map/LinearMap/Builder/LinearMapBuilder.hpp"

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

		const uint32 facePositionOffset = facePositionCount;
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
			.textureFiltering = descriptor.textureFiltering,
			.lightmap = descriptor.lightmap,
		});

		const uint32 ceilingFaceIndices[] = {
			facePositionOffset + 3,
			facePositionOffset + 2,
			facePositionOffset + 1,
			facePositionOffset + 0,
		};

		createFace({
			.indices = ceilingFaceIndices,
			.normal = {0.0f, -1.0f, 0.0f},
			.material = descriptor.material,
			.textureFiltering = descriptor.textureFiltering,
			.lightmap = descriptor.lightmap,
		});

		extrudeFace(floorFace, -height);

		rooms.emplace_back(room);

		return static_cast<uint32>(rooms.size() - 1);
	}
}