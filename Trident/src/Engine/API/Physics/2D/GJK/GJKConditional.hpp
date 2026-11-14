// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/2D/RigidBody.hpp"

namespace Trident {
	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	// Returns whether A contains p.
	// NOTE: In this implementation, `a` is the name of the most recent vertex added to the simplex.
	bool GJKConditional2D(const RigidBody2D& A, vec2<float32> p, const Hull2D& shapeA);
}