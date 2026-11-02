// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/API/Physics/3D/Simplex.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	struct GJK3DOutput {
		/// Final simplex.
		Simplex3 simplex;

		/// Closest point on A.
		vec3<float32> closestA;

		/// Closest point on B.
		vec3<float32> closestB;

		/// Minimum distance between `closestA` and `closestB`.
		/// A and B are intersecting if the distance is <= 0.
		float32 distance;
	};

	/// Extension of the Gilbert-Johnson-Keerthi algorithm in 3D based on Erin Catto's distance implementation.
	/// A must be a point hull and B a polygon hull.
	GJK3DOutput GJKPointPolygon3D(const RigidBody3DComponent& A, const RigidBody3DComponent& B, const Hull3D& hullA, const Hull3D& hullB);
}