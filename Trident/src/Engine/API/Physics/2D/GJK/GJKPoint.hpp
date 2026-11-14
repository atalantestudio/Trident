// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/RigidBody.hpp"
#include "Engine/API/Physics/2D/Shape/Shape.hpp"
#include "Engine/API/Physics/2D/Simplex.hpp"

namespace Trident {
	// The result of GJK in 2D.
	struct GJKPointOutput2D {
		// Final simplex.
		Simplex2 simplex;

		// Closest point on A.
		vec2<float32> closestA;

		// Minimum distance between A and B.
		// Calculated as the length of the difference between closestA and closestB.
		// A and B are intersecting if the distance is <= 0.
		float32 distance;

		// Last search direction used by GJK.
		vec2<float32> separatingAxis;
	};

	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	GJKPointOutput2D GJKPoint2D(const RigidBody2D& A, vec2<float32> p, const Shape2D& shapeA);
}