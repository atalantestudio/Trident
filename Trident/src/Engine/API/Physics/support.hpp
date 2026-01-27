// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/RigidBody2D.hpp"

namespace Trident {
	// TODO: Some geometries (e.g. CircleGeometry2D) don't implement `supportIndex`. Prefer `supportVertex` instead.
	// Returns the index of the farthest point on `A` along `d`, in local space.
	static uint32 supportIndex(const RigidBody2D& A, vec2<float32> d) {
		const vec2<float32> dR = d * mat2x2<float32>::rotation(-A.rotation);

		return A.geometry->supportIndex(dR);
	}

	// Returns the farthest point on `A` along `d`, in local space.
	static vec2<float32> supportVertex(const RigidBody2D& A, vec2<float32> d) {
		const vec2<float32> dR = d * mat2x2<float32>::rotation(-A.rotation);

		return A.geometry->support(dR);
	}

	// Returns the farthest point on `A` along `d`, in world-space.
	static vec2<float32> support(const RigidBody2D& A, vec2<float32> d) {
		return multiplyTransform(supportVertex(A, d), A.transform);
	}
}