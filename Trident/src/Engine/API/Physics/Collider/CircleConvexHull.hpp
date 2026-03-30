// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/SATCircleConvexHull2D.hpp"

namespace Trident {
	static bool CircleConvexHullCollider(
		const RigidBody2D& A,
		const RigidBody2D& B,
		const Geometry2D& geometryA,
		const Geometry2D& geometryB,
		ContactManifold2D* contactManifold
	) {
		const Geometry2D pointGeometry = Geometry2D::pointAtOrigin();

		// The scaling factor is assumed to be the same in both axes.
		const float32 scaleA = A.scale[0];

		const GJK2DOutput gjkOutput = GJK2D(A, B, pointGeometry, geometryB);
		const float32 d = gjkOutput.distance - geometryA.radius * scaleA;

		if (d > 0.0f) {
			return false;
		}

		if (d > -geometryA.radius * scaleA) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		const SAT2DOutput satOutput = SATCircleConvexHull2D(A, B, geometryA, geometryB);

		contactManifold->normal = -satOutput.normal;
		contactManifold->contactCount = 1;
		contactManifold->contacts[0].position = satOutput.incidentSupportPoint - satOutput.normal * satOutput.separation;
		contactManifold->contacts[0].separation = satOutput.separation - geometryA.radius * scaleA;

		return true;
	}
}