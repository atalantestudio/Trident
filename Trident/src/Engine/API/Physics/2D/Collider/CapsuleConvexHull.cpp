// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Collider/Collider.hpp"

namespace Trident {
	bool CapsuleConvexHull2D(const Shape2D& shapeA, const Shape2D& shapeB, const RigidBody2D& rigidBodyA, const RigidBody2D& rigidBodyB, ContactManifold2D* contactManifold) {
		DEBUG_ASSERT(shapeA.type == ShapeType2D::CAPSULE);
		DEBUG_ASSERT(shapeB.type == ShapeType2D::CONVEX_HULL);

		// TODO(physics/2d/collider/capsule-convex-hull)
		ENGINE_ERROR_UNIMPLEMENTED();

		return false;
	}
}