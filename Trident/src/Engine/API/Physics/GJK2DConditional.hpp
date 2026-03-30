// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/support.hpp"

namespace Trident {
	static constexpr uint8 GJK_2D_CONDITIONAL_ITERATION_LIMIT = 8;

	static void handleSimplex1(sim2<float32>& s, vec2<float32>& d) {
		const vec2<float32> a = s[1];
		const vec2<float32> b = s[0];
		const vec2<float32> ab = b - a;
		const vec2<float32> ao = -a;

		d = cross2d(ab, ao, ab);
	}

	static bool handleSimplex2(sim2<float32>& s, vec2<float32>& d) {
		const vec2<float32> a = s[2];
		const vec2<float32> b = s[1];
		const vec2<float32> c = s[0];
		const vec2<float32> ab = b - a;
		const vec2<float32> ac = c - a;
		const vec2<float32> ao = -a;
		const vec2<float32> abPerp = cross2d(ac, ab, ab);
		const vec2<float32> acPerp = cross2d(ab, ac, ac);

		if (dot(ao, abPerp) > 0.0f) {
			s = {a, b};
			d = abPerp;

			return false;
		}

		if (dot(ao, acPerp) > 0.0f) {
			s = {a, c};
			d = acPerp;

			return false;
		}

		return true;
	}

	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	// Returns true if the Minkowski difference of A and B contains the origin, and false otherwise.
	// NOTE: In this implementation, `a` is the name of the most recent vertex added to the simplex.
	static bool GJK2DConditional(const RigidBody2D& A, const RigidBody2D& B, const Geometry2D& geometryA, const Geometry2D& geometryB) {
		vec2<float32> d = B.position - A.position;

		// Degenerate case: The objects have the same position.
		if (lengthSquared(d) == 0.0f) {
			return true;
		}

		// Find a support point on the Minkowski difference of A and B.
		vec2<float32> a = support(A, geometryA, d) - support(B, geometryB, -d);

		// Check if `a` passes the origin.
		if (dot(a, d) < 0.0f) {
			return false;
		}

		sim2<float32> s = {a};

		d = -a;

		uint8 iteration = 0;

		while (iteration < GJK_2D_CONDITIONAL_ITERATION_LIMIT) {
			// Find a support point on the Minkowski difference of A and B.
			a = support(A, geometryA, d) - support(B, geometryB, -d);

			// Check if `a` passes the origin.
			if (dot(a, d) < 0.0f) {
				return false;
			}

			s[s.count] = a;
			s.count += 1;

			if (s.count == 3) {
				if (handleSimplex2(s, d)) {
					return true;
				}
			}
			else {
				handleSimplex1(s, d);
			}

			iteration += 1;
		}

		ENGINE_WARNING("GJK 2D (conditional): Iteration limit reached.");

		return false;
	}
}