// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/RigidBody2D.hpp"

namespace Trident {
	// TODO: Some geometries (e.g. CircleGeometry2D) don't implement `supportIndex`. Prefer `supportVertex` instead.
	// Returns the index of the farthest point on `A` along `d`, in local space.
	static uint32 supportIndex(const RigidBody2D& A, vec2<float32> d) {
		// TODO: Check if R is correct.
		const mat2x2<float32> R = mat2x2<float32>::rotation(-A.rotation);

		return A.geometry->supportIndex(d * R);
	}

	// Returns the farthest point on `A` along `d`, in local space.
	static vec2<float32> supportVertex(const RigidBody2D& A, vec2<float32> d) {
		const mat2x2<float32> R = mat2x2<float32>::rotation(A.rotation);

		return A.geometry->support(d * R);
	}

	// Returns the farthest point on `A` along `d`, in world-space.
	static vec2<float32> support(const RigidBody2D& A, vec2<float32> d) {
		const vec2<float32> s = supportVertex(A, d);

		return multiplyTransform(s, A.transform);
	}
}