// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	void MapBuilder::extrudeFace(Face& face, vec3<float32> direction, float32 scale) {
		if (abs(scale) < 1e-4) {
			return;
		}

		const uint32 indexCount = face.getIndexCount();

		vec3<float32> a = facePositions[face.indices[0]];
		uint32 indexA = 0;

		while (indexA < indexCount) {
			facePositions[facePositionCount + indexA] = a + direction * scale;

			const uint32 indexB = (indexA + 1) % indexCount;
			const vec3<float32> b = facePositions[face.indices[indexB]];
			const uint32 extrudedFaceIndices[] = {
				facePositionCount + indexB,
				facePositionCount + indexA,
				face.indices[indexA],
				face.indices[indexB],
			};

			createFace({
				.indices = extrudedFaceIndices,
				.normal = normalized(cross(b - a, direction * scale)),
				.material = face.material,
				.textureFiltering = face.textureFiltering,
				.lightmap = face.lightmap,
			});

			a = b;
			indexA += 1;
		}

		indexA = 0;

		while (indexA < indexCount) {
			face.indices[indexA] = facePositionCount + indexA;

			indexA += 1;
		}

		facePositionCount += face.getUniqueIndexCount();
	}
}