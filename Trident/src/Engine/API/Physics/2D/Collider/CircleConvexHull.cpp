// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/2D/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/2D/GJK/GJK.hpp"
#include "Engine/API/Physics/2D/SAT/SATCircleConvexHull.hpp"

namespace Trident {
	bool CircleConvexHull2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* manifold) {
		const Hull2D pointHull = Hull2D::point();

		// The scaling factor is assumed to be the same in both axes.
		const float32 scaleA = rigidBodyA.scale[0];

		const GJK2DOutput gjkOutput = GJK2D(rigidBodyA, rigidBodyB, pointHull, shapeB);
		const float32 d = gjkOutput.distance - shapeA.radius * scaleA;

		if (d > 0.0f) {
			return false;
		}

		if (d > -shapeA.radius * scaleA) {
			manifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			manifold->contactCount = 1;
			manifold->contacts[0].position = gjkOutput.closestB;
			manifold->contacts[0].separation = d;

			return true;
		}

		const SATCircleConvexHullOutput2D satOutput = SATCircleConvexHull2D(rigidBodyA, rigidBodyB, shapeA, shapeB);

		manifold->normal = -satOutput.axis;
		manifold->contactCount = 1;
		manifold->contacts[0].position = satOutput.incidentSupportPoint - satOutput.axis * satOutput.separation;
		manifold->contacts[0].separation = satOutput.separation - shapeA.radius * scaleA;

		return true;
	}
}