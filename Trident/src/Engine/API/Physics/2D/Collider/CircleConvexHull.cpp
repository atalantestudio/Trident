// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"
#include "Engine/API/Physics/2D/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/2D/GJK/GJKPoint.hpp"
#include "Engine/API/Physics/2D/SAT/SATCircleConvexHull.hpp"

namespace Trident {
	bool CircleConvexHull2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* manifold) {
		const GJKPointOutput2D gjkOutput = GJKPoint2D(rigidBodyB, rigidBodyA.position, shapeB);
		const float32 d = gjkOutput.distance - shapeA.circle.radius;

		if (d > 0.0f) {
			return false;
		}

		if (d > -shapeA.circle.radius) {
			manifold->normal = normalized(gjkOutput.closestA - rigidBodyA.position);
			manifold->contactCount = 1;

			Contact2D& contact = manifold->contacts[0];
			contact.position = gjkOutput.closestA;
			contact.separation = d;

			return true;
		}

		const SATCircleConvexHullOutput2D satOutput = SATCircleConvexHull2D(rigidBodyA, rigidBodyB, shapeA, shapeB);

		manifold->normal = -satOutput.axis;
		manifold->contactCount = 1;

		Contact2D& contact = manifold->contacts[0];
		contact.position = satOutput.incidentSupportPoint - satOutput.axis * satOutput.separation;
		contact.separation = satOutput.separation - shapeA.circle.radius;

		return true;
	}
}