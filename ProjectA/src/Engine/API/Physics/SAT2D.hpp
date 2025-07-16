#pragma once

#include "Engine/API/Object/PhysicalObject.hpp"

namespace ProjectA {
	struct SAT2DOutput {
		// If the separation is negative, the shapes are intersecting.
		float32 separation;
	};

	SAT2DOutput SAT2D(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		SAT2DOutput output;
		output.separation = 0.0f;

		BREAKPOINT();

		return output;
	}
}