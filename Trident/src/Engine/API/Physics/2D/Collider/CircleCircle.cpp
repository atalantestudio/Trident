// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"

namespace Trident {
	bool CircleCircle2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* contactManifold) {
		const vec2<float32> ab = rigidBodyB.position - rigidBodyA.position;
		const float32 lengthSquaredAB = lengthSquared(ab);

		if (abs(lengthSquaredAB) < 0.001f) {
			// Degenerate case: The circles are coincident.
			return false;
		}

		const float32 rArB = shapeA.circle.radius + shapeB.circle.radius;
		const float32 d = lengthSquaredAB - rArB * rArB;

		if (d > 0.0f) {
			return false;
		}

		contactManifold->normal = normalized(ab);
		contactManifold->contactCount = 1;

		const vec2<float32> sA = rigidBodyA.position - contactManifold->normal * shapeA.circle.radius;
		const vec2<float32> sB = rigidBodyB.position + contactManifold->normal * shapeB.circle.radius;

		Contact2D& contact = contactManifold->contacts[0];
		contact.position = (sA + sB) * 0.5f;
		contact.separation = d;

		return true;
	}
}