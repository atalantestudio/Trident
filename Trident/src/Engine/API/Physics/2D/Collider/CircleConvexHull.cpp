// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"
#include "Engine/API/Physics/2D/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/2D/GJK2D.hpp"
#include "Engine/API/Physics/2D/SAT2D.hpp"

namespace Trident {
	bool CircleConvexHull2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* contactManifold) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CIRCLE);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CONVEX_HULL);

		const GJK2DOutput gjkOutput = GJK2D(rigidBodyA, rigidBodyB, shapeA, shapeB);
		const float32 d = gjkOutput.distance - shapeA.circle.radius;

		if (d > 0.0f) {
			return false;
		}

		if (d > -shapeA.circle.radius) {
			contactManifold->normal = normalized(gjkOutput.closestB - gjkOutput.closestA);
			contactManifold->contactCount = 1;
			contactManifold->contacts[0].position = gjkOutput.closestB;
			contactManifold->contacts[0].separation = d;

			return true;
		}

		const SATCircleConvexHullOutput2D satOutput = SATCircleConvexHull2D(rigidBodyA, rigidBodyB, shapeA, shapeB);

		contactManifold->normal = -satOutput.axis;
		contactManifold->contactCount = 1;

		Contact2D& contact = contactManifold->contacts[0];
		contact.position = satOutput.incidentSupportPoint - satOutput.axis * satOutput.separation;
		contact.separation = satOutput.separation - shapeA.circle.radius;

		return true;
	}
}