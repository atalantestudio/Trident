#pragma once

#include "Engine/API/Object/PhysicalObject.hpp"

namespace ProjectA {
	// TODO: Note that some geometries (e.g. CircleGeometry2D) don't implement `supportIndex`.
	//       Please use `supportVertex` instead.
	// Returns the index of the farthest point on `copy` along `d`, in model-space.
	static uint32 supportIndex(const PhysicalObjectCopy2D& copy, const vec2<float32>& d) {
		const vec2<float32> dR = rotate(d, -copy.rotation);

		return copy.geometry->supportIndex(dR);
	}

	// Returns the farthest point on `copy` along `d`, in model-space.
	static vec2<float32> supportVertex(const PhysicalObjectCopy2D& copy, const vec2<float32>& d) {
		const vec2<float32> dR = rotate(d, -copy.rotation);

		return copy.geometry->support(dR);
	}

	// Returns the farthest point on `copy` along `d`, in world-space.
	static vec2<float32> support(const PhysicalObjectCopy2D& copy, const vec2<float32>& d) {
		return multiplyTransform(supportVertex(copy, d), copy.transform);
	}
}