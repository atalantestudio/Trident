// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"

namespace Trident {
	static bool CircleConvexHullCollider(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB, ContactManifold2D* contactManifold) {
		const Hull2D pointHull = Hull2D::point();

		// The scaling factor is assumed to be the same in both axes.
		const float32 scaleA = A.scale[0];

		const GJK2DOutput gjkOutput = GJK2D(A, B, pointHull, hullB);
		const float32 d = gjkOutput.distance - hullA.getRadius() * scaleA;

		if (d > 0.0f) {
			return false;
		}

		if (d > -hullA.getRadius() * scaleA) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		const SAT2DOutput satOutput = SATCircleConvexHull2D(A, B, hullA, hullB);

		contactManifold->normal = -satOutput.normal;
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = satOutput.incidentSupportPoint - satOutput.normal * satOutput.separation;
		contactManifold->contacts[0].separation = satOutput.separation - hullA.getRadius() * scaleA;

		return true;
	}
}