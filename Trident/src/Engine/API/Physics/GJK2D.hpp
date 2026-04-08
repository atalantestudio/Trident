// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Physics/Simplex.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	// The result of GJK in 2D.
	struct GJK2DOutput {
		// Final simplex.
		Simplex2 simplex;

		// Closest point on A.
		vec2<float32> closestA;

		// Closest point on B.
		vec2<float32> closestB;

		// Minimum distance between A and B.
		// Calculated as the length of the difference between closestA and closestB.
		// A and B are intersecting if the distance is <= 0.
		float32 distance;

		// Last search direction used by GJK.
		vec2<float32> separatingAxis;

		// Closest feature on A. Can either be a vertex or an edge.
		Feature2D closestFeatureA;

		// Closest feature on B. Can either be a vertex or an edge.
		Feature2D closestFeatureB;
	};

	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	GJK2DOutput GJK2D(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB);
}