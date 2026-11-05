// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"
#include "Engine/API/Physics/2D/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/2D/GJK2D.hpp"
#include "Engine/API/Physics/2D/SAT2D.hpp"

namespace Trident {
	bool CircleConvexHull2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* contactManifold) {
		// TODO(physics/2d/collider/circle-convex-hull)
		ENGINE_ERROR_UNIMPLEMENTED();

		return false;
	}

	/* bool CircleConvexHullCollider(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB, ContactManifold2D* contactManifold) {
		const Hull2D pointHull = Hull2D::point();

		// The scaling factor is assumed to be the same in both axes.
		const float32 scaleA = A.scale[0];

		const GJK2DOutput gjkOutput = GJK2D(A, B, pointHull, hullB);
		const float32 d = gjkOutput.distance - hullA.radius * scaleA;

		if (d > 0.0f) {
			return false;
		}

		if (d > -hullA.radius * scaleA) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		const SAT2DOutput satOutput = SATCircleConvexHull2D(A, B, hullA, hullB);

		contactManifold->normal = -satOutput.axis;
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = satOutput.incidentSupportPoint - satOutput.axis * satOutput.separation;
		contactManifold->contacts[0].separation = satOutput.separation - hullA.radius * scaleA;

		return true;
	} */
}