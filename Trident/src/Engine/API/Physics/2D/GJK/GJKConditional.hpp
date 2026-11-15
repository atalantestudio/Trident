// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/RigidBody.hpp"
#include "Engine/API/Physics/2D/Shape/Shape.hpp"

namespace Trident {
	/// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	/// Returns whether `A` contains `p`.
	bool GJKConditional2D(const RigidBody2D& A, vec2<float32> p, const Shape2D& shapeA);
}