// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/2D/Contact/ContactManifold.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	typedef bool (*Collider2D)(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold);

	bool CircleCircle2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold);
	// bool CircleCapsule2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold);
	bool CircleConvexHull2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold);
	// bool CapsuleCapsule2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold);
	// bool CapsuleConvexHull2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold);
	bool ConvexHullConvexHull2D(const Hull2D& shapeA, const Hull2D& shapeB, const RigidBody2DComponent& rigidBodyA, const RigidBody2DComponent& rigidBodyB, ContactManifold2D* manifold);
}