// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider2D.hpp"

namespace Trident {
	bool CircleCircle2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* contactManifold) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CIRCLE);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CIRCLE);

		const vec2<float32> a = rigidBodyA.position;
		const vec2<float32> b = rigidBodyB.position;
		const vec2<float32> ab = b - a;

		const float32 rA = shapeA.circle.radius;
		const float32 rB = shapeB.circle.radius;
		const float32 r = rA + rB;

		const float32 dd = lengthSquared(ab);

		if (dd > r * r) {
			return false;
		}

		// Handle coincident circles.
		if (abs(dd) < 0.001f) {
			return false;
		}

		const float32 d = sqrt(dd);
		const vec2<float32> normal = ab / d;

		contactManifold->normal = normal;
		contactManifold->contactCount = 1;

		Contact2D& contact = contactManifold->contacts[0];
		contact.position = (a + normal * rA + b - normal * rB) * 0.5f;
		contact.separation = d - r;

		return true;
	}
}