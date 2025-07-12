#pragma once

#include "Engine/API/Geometry/Geometry2DType.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	typedef void (*Collider2D)(const Scene2D& scene, const ref<RigidBody>& rigidBodyA, const ref<RigidBody>& rigidBodyB);

	static Collider2D CircleCircleCollider;
	static Collider2D CircleConvexHullCollider;
	static Collider2D ConvexHullConvexHullCollider;

	static constexpr Collider2D COLLIDER_2D_TABLE[Geometry2DType::MAX_ENUM][Geometry2DType::MAX_ENUM] = {
		CircleCircleCollider, CircleConvexHullCollider,
		nullptr,              ConvexHullConvexHullCollider,
	};

	static void CircleCircleCollider(const Scene2D& scene, const ref<RigidBody>& rigidBodyA, const ref<RigidBody>& rigidBodyB) {
		// TODO
	}

	static void CircleConvexHullCollider(const Scene2D& scene, const ref<RigidBody>& rigidBodyA, const ref<RigidBody>& rigidBodyB) {
		// TODO
	}

	static void ConvexHullConvexHullCollider(const Scene2D& scene, const ref<RigidBody>& rigidBodyA, const ref<RigidBody>& rigidBodyB) {
		// TODO
	}
}