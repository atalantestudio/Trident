// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Face3D {
		svec3<float32> normal;
		float32 offset;

		// TODO: The count should be a uint8, as a face cannot be referred by more than 255 edges.
		range<uint16, uint16> edgeIndexRange;
	};

	/// See https://stackoverflow.com/questions/7685495/transforming-a-3d-plane-using-a-4x4-matrix.
	inline Face3D transformFace(const Face3D& F, const mat4x4<float32>& M) {
		const vec3<float32> n = F.normal;
		const vec3<float32> o = multiplyTransform(n * F.offset, M);
		const vec3<float32> n2 = normalized(multiplyTransform(n, transpose(inverse(M))));

		return {
			.normal = svec3<float32>(n2),
			.offset = dot(o, n2),
		};
	}
}