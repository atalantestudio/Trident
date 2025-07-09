#pragma once

#include "Engine/API/Scene/Scene2D.hpp"

namespace ProjectA {
	static vec2<float32> cancelRotation(float32 rotation, const vec2<float32>& d) {
		const mat3x3<float32> R = mat3x3<float32>::rotation(-rotation);
		const vec2<float32> R0 = {R[0][0], R[1][0]};
		const vec2<float32> R1 = {R[0][1], R[1][1]};

		return {dot(R0, d), dot(R1, d)};
	}

	// Restore the rotation of A onto the support point.
	static vec2<float32> transformSupport(const PhysicalObjectCopy2D& copy, uint32 supportIndex) {
		const vec2<float32>& a = copy.geometry->vertices[supportIndex];

		return multiplyTransform(a, copy.transform);
	}

	// Returns the index of the farthest point of A in the direction d.
	inline uint32 supportIndex(const PhysicalObjectCopy2D& copy, const vec2<float32>& d) {
		const vec2<float32> dR = cancelRotation(copy.rotation, d);

		return copy.geometry->support(dR);
	}

	// Returns the farthest point of A in the direction d.
	inline vec2<float32> support(const PhysicalObjectCopy2D& copy, const vec2<float32>& d) {
		const uint32 i = supportIndex(copy, d);

		return transformSupport(copy, i);
	}
}