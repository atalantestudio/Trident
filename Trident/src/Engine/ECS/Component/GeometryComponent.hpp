// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO: Use in the scene ECS.
	/// References a geometry in a contiguous buffer.
	struct GeometryComponent {
		uint32 geometryIndex;
	};
}