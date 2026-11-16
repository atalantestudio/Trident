// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"

namespace Trident {
	bool CircleCircle2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* manifold) {
		static constexpr float32 COINCIDENT_CIRCLE_BIAS = 0.001f;

		DEBUG_ASSERT(shapeA.type == ShapeType2D::CIRCLE);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CIRCLE);

		const vec2<float32> a = rigidBodyA.position;
		const vec2<float32> b = rigidBodyB.position;
		const vec2<float32> ab = b - a;

		const float32 dd = lengthSquared(ab);

		const float32 rA = shapeA.circle.radius;
		const float32 rB = shapeB.circle.radius;
		const float32 r = rA + rB;

		if (dd > r * r) {
			return false;
		}

		// Degenerate case (coincident circles).
		if (dd < COINCIDENT_CIRCLE_BIAS) {
			return false;
		}

		const float32 d = sqrt(dd);
		const vec2<float32> normal = ab / d;

		manifold->normal = normal;
		manifold->contactCount = 1;

		Contact2D& contact = manifold->contacts[0];
		contact.position = (a + normal * rA + b - normal * rB) * 0.5f;

		// TODO(physics/2d/circle-circle)
		// Using the squared values instantly separes the bodies with a bouncy effect.
		// d - r is the correct calculation but the separation takes too much time.
		contact.separation = dd - r * r;

		return true;
	}
}