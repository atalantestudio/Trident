// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/3D/ContactManifold3D.hpp"

namespace Trident {
	struct ContactPair3D {
		ContactManifold3D manifold;

		// TODO(physics/3d/friction-types): Differentiate static and dynamic friction.
		float32 combinedFriction;
	};
}