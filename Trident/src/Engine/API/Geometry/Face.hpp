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
}