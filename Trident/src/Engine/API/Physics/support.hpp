// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	// TODO: Some geometries (e.g. CircleGeometry2D) don't implement `supportIndex`. Use `supportVertex` instead.
	// Returns the index of the farthest point on `A` along `d`, in local space.
	inline uint32 supportIndex(const RigidBody2DComponent& A, const Hull2D& hull, vec2<float32> d) {
		// TODO: Check if R is correct.
		const mat2x2<float32> R = mat2x2<float32>::rotation(-A.rotation);

		return hull.supportIndex(d * R);
	}

	// Returns the farthest point on `A` along `d`, in local space.
	inline vec2<float32> supportVertex(const RigidBody2DComponent& A, const Hull2D& hull, vec2<float32> d) {
		const mat2x2<float32> R = mat2x2<float32>::rotation(A.rotation);

		return hull.support(d * R);
	}

	// Returns the farthest point on `A` along `d`, in world-space.
	inline vec2<float32> support(const RigidBody2DComponent& A, const Hull2D& hull, vec2<float32> d) {
		const vec2<float32> s = supportVertex(A, hull, d);

		return multiplyTransform(s, A.world);
	}
}