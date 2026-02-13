// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct ClusteredInstance {
		mat4x4<float32> transform;
		uint32 geometryIndex;
		uint32 materialIndex;
		uint32 materialReferenceIndex;

		// Use as an offset in the index buffer to access the start of the geometry used by this instance.
		sint32 clusterOffset;
	};
}