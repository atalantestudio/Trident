#pragma once

#include "Engine/API/Physics/support.hpp"

namespace ProjectA {
	static constexpr uint8 GJK_2D_CONDITIONAL_ITERATION_LIMIT = 8;
	static constexpr uint8 GJK_2D_POINT_CONDITIONAL_ITERATION_LIMIT = 8;

	static void handleSimplex1(sim2<float32>& s, vec2<float32>& d) {
		// TODO: Use structured bindings here, e.g.:
		// const vec2<float32>& [b, a] = s;

		const vec2<float32>& a = s[1];
		const vec2<float32>& b = s[0];
		const vec2<float32> ab = b - a;
		const vec2<float32> ao = -a;

		if (dot(ab, ao) > 0.0f) {
			d = cross2d(ab, ao, ab);

			return;
		}

		s = {a};
		d = ao;
	}

	static bool handleSimplex2(sim2<float32>& s, vec2<float32>& d) {
		// TODO: Use structured bindings here, e.g.:
		// const vec2<float32>& [c, b, a] = s;

		const vec2<float32>& a = s[2];
		const vec2<float32>& b = s[1];
		const vec2<float32>& c = s[0];
		const vec2<float32> ab = b - a;
		const vec2<float32> ac = c - a;
		const vec2<float32> ao = -a;
		const vec2<float32> abc = cross2d(ab, ac);

		if (dot(cross2d(abc, ac), ao) > 0.0f) {
			if (dot(ac, ao) > 0.0f) {
				s = {a, c};
				d = cross2d(ac, ao, ac);

				return false;
			}

			s = {a, b};

			handleSimplex1(s, d);

			return false;
		}

		if (dot(cross2d(ab, abc), ao) > 0.0f) {
			s = {a, b};

			handleSimplex1(s, d);

			return false;
		}

		return true;
	}

	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	// Returns true if the Minkowski difference of A and B contains the origin, and false otherwise.
	// References:
	// 
	// - Reducible - A Strange But Elegant Approach to a Surprisingly Hard Problem (GJK Algorithm)
	//   https://youtu.be/ajv46BSqcK4
	// - Iain Winter - GJK: Collision detection algorithm in 2D/3D
	//   https://winter.dev/articles/gjk-algorithm
	// 
	// NOTE: In this implementation, `a` is the name of the most recent vertex added to the simplex.
	static bool GJK2DConditional(const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		vec2<float32> d = B.position - A.position;

		// Degenerate case: The objects have the same position.
		if (lengthSq(d) == 0.0f) {
			return true;
		}

		// Find a support point on the Minkowski difference of A and B.
		vec2<float32> a = support(B, d) - support(A, -d);

		// Check if `a` passes the origin.
		if (dot(a, d) < 0.0f) {
			return false;
		}

		sim2<float32> s = {a};

		d = -a;

		for (uint8 i = 0; i < GJK_2D_CONDITIONAL_ITERATION_LIMIT; i += 1) {
			// Find a support point on the Minkowski difference of A and B.
			a = support(B, d) - support(A, -d);

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

			handleSimplex1(s, d);
		}

		ENGINE_WARN("GJK 2D (conditional): Iteration limit reached.");

		return false;
	}

	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	// Returns true if A contains `p`.
	// References:
	// 
	// - Casey Muratori - Implementing GJK (2006)
	//   https://caseymuratori.com/blog_0003
	// - Reducible - A Strange But Elegant Approach to a Surprisingly Hard Problem (GJK Algorithm)
	//   https://youtu.be/ajv46BSqcK4
	// 
	// NOTE: In this implementation, `a` is the name of the most recent vertex added to the simplex.
	static bool GJK2DPointConditional(const PhysicalObjectCopy2D& A, const vec2<float32>& p) {
		vec2<float32> d = p - A.position;

		// Degenerate case: The objects have the same position.
		if (lengthSq(d) < 0.001f) {
			return true;
		}

		// Find a support point on the Minkowski difference of A and B.
		vec2<float32> a = p - support(A, -d);

		// Check if `a` passes the origin.
		if (dot(a, d) < 0.0f) {
			return false;
		}

		sim2<float32> s = {a};

		d = -a;

		for (uint8 i = 0; i < GJK_2D_POINT_CONDITIONAL_ITERATION_LIMIT; i += 1) {
			// Find a support point on the Minkowski difference of A and B.
			a = p - support(A, -d);

			// Check if `a` passes the origin.
			if (dot(a, d) < 0) {
				return false;
			}

			s[s.count] = a;
			s.count += 1;

			if (s.count == 3) {
				if (handleSimplex2(s, d)) {
					return true;
				}
			}

			handleSimplex1(s, d);
		}

		ENGINE_WARN("GJK 2D (point, conditional): Iteration limit reached.");

		return false;
	}
}