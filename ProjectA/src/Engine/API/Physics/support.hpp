#pragma once

#include "Engine/API/Object/PhysicalObject.hpp"

namespace ProjectA {
	// Rotates `d` by `r`.
	static vec2<float32> rotate(const vec2<float32>& d, float32 r) {
		const mat3x3<float32> R = mat3x3<float32>::rotation(r);
		const vec2<float32> R0 = {R[0][0], R[1][0]};
		const vec2<float32> R1 = {R[0][1], R[1][1]};

		return {dot(R0, d), dot(R1, d)};
	}

	// Returns the index of the farthest point of A in the direction d.
	inline uint32 supportIndex(const PhysicalObjectCopy2D& copy, const vec2<float32>& d) {
		const vec2<float32> dR = rotate(d, -copy.rotation);

		return copy.geometry->supportIndex(dR);
	}

	// Returns the farthest point of A in the direction d.
	inline vec2<float32> support(const PhysicalObjectCopy2D& copy, const vec2<float32>& d) {
		const uint32 index = supportIndex(copy, d);

		return multiplyTransform(copy.geometry->vertices[index], copy.transform);
	}
}