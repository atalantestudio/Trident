// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"

namespace Trident {
	struct Geometry3DComponent {
		/// References a geometry in a contiguous buffer.
		uint32 geometryIndex;

		bool hasHull = false;
		Hull3D hull;
	};
}