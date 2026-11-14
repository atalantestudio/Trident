// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/2D/RigidBody.hpp"

namespace Trident {
	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	// Returns true if the Minkowski difference of A and B contains the origin, and false otherwise.
	// NOTE: In this implementation, `a` is the name of the most recent vertex added to the simplex.
	bool GJK2DConditional(const RigidBody2D& A, const RigidBody2D& B, const Hull2D& hullA, const Hull2D& hullB);
}