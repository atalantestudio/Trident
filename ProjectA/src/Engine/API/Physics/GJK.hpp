#pragma once

#include "Engine/API/Object/Object2D.hpp"
#include "Engine/API/Physics/support.hpp"

namespace ProjectA {
	static constexpr uint8 GJK_BASE_MAX_ITERATIONS = 16;

	// TODO: Refactor using sign flip for 2D perpendicular vectors.
	static vec2<float32> cross2d(const vec2<float32>& a, const vec2<float32>& b) {
		const vec3<float32> a_3d = {a[0], a[1], 0.0f};
		const vec3<float32> b_3d = {b[0], b[1], 0.0f};
		const vec3<float32> r_3d = cross(a_3d, b_3d);

		return {r_3d[0], r_3d[1]};
	}

	// TODO: Refactor using sign flip for 2D perpendicular vectors.
	static vec2<float32> tripleCross2d(const vec2<float32>& a, const vec2<float32>& b, const vec2<float32>& c) {
		const vec2<float32> b_c = {b[0] * c[1], b[1] * c[0]};

		return {
			a[1] * (b_c[0] - b_c[1]),
			a[0] * (b_c[1] - b_c[0]),
		};
	}

	static void handle1Simplex(sim2<float32>& s, vec2<float32>& d) {
		const vec2<float32>& a = s[1];
		const vec2<float32>& b = s[0];
		const vec2<float32> ab = b - a;
		const vec2<float32> ao = -a;

		if (dot(ab, ao) > 0.0f) {
			d = tripleCross2d(ab, ao, ab);

			return;
		}

		s = {a};
		d = ao;
	}

	static bool handle2Simplex(sim2<float32>& s, vec2<float32>& d) {
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
				d = tripleCross2d(ac, ao, ac);

				return false;
			}

			s = {a, b};

			handle1Simplex(s, d);

			return false;
		}

		if (dot(cross2d(ab, abc), ao) > 0.0f) {
			s = {a, b};

			handle1Simplex(s, d);

			return false;
		}

		return true;
	}

	// Gilbert-Johnson-Keerthi algorithm (distance-less) implementation in 2D.
	// Returns true if the Minkowski difference of A and B contains the origin, and false otherwise.
	// 
	// References:
	// 
	// - Reducible - A Strange But Elegant Approach to a Surprisingly Hard Problem (GJK Algorithm)
	//   https://youtu.be/ajv46BSqcK4
	// - Casey Muratori - Implementing GJK (2006)
	//   https://caseymuratori.com/blog_0003
	// - Erin Catto - Computing Distance using GJK - GDC 2010
	//   https://box2d.org/files/ErinCatto_GJK_GDC2010.pdf
	//   https://box2d.org/files/ErinCatto_GJK_Source.zip
	// - William Bittle - GJK, Distance, Closest Points
	//   https://dyn4j.org/2010/04/gjk-distance-closest-points
	// - Iain Winter - GJK: Collision detection algorithm in 2D/3D
	//   https://winter.dev/articles/gjk-algorithm
	// - Christer Ericson - Real-Time Collision Detection
	//   https://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
	// 
	// In this implementation, a is the name of the most recent vertex added to the simplex.
	inline bool GJK2DConditional(const Object2D& A, const Object2D& B) {
		// The position is the offset from the object's center of mass.
		// TODO: Normalize or not?
		vec2<float32> d = B.position - A.position;

		// TODO: Do A and B always intersect in this case?
		if (lengthSq(d) == 0.0f) {
			return true;
		}

		// Find a support point on the Minkowski difference of A and B.
		vec2<float32> a = support(B, d) - support(A, -d);

		// Check if a passes the origin.
		if (dot(a, d) < 0.0f) {
			return false;
		}

		sim2<float32> s = {a};

		d = -a;

		for (uint8 iteration = 0; iteration < GJK_BASE_MAX_ITERATIONS; iteration += 1) {
			// Find a support point on the Minkowski difference of A and B.
			a = support(B, d) - support(A, -d);

			// Check if a passes the origin.
			if (dot(a, d) < 0.0f) {
				return false;
			}

			s += a;

			switch (s.count) {
				case 2: {
					handle1Simplex(s, d);

					break;
				}
				case 3:
					if (handle2Simplex(s, d)) {
						return true;
					}

					break;
				default:
					ENGINE_ASSERT(false, "Invalid simplex size.");
			}
		}

		ENGINE_WARN("Max iteration limit reached.");

		return false;
	}
}