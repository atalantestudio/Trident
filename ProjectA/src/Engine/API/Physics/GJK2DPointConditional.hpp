#pragma once

#include "Engine/API/Object/Object2D.hpp"
#include "Engine/API/Physics/support.hpp"

namespace ProjectA {
	static constexpr uint8 GJK_2D_POINT_CONDITIONAL_ITERATION_LIMIT = 8;

	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	// Returns true if A contains p, and false otherwise.
	// References:
	// 
	// - Reducible - A Strange But Elegant Approach to a Surprisingly Hard Problem (GJK Algorithm)
	//   https://youtu.be/ajv46BSqcK4
	// - Iain Winter - GJK: Collision detection algorithm in 2D/3D
	//   https://winter.dev/articles/gjk-algorithm
	//   https://youtu.be/MDusDn8oTSE
	// 
	inline bool GJK2DPointConditional(const Object2D& A, const vec2<float32>& p) {
		vec2<float32> d = p - A.position;

		if (lengthSq(d) == 0.0f) {
			return true;
		}

		// In this implementation, a is the name of the most recent vertex added to the simplex.
		vec2<float32> a = p - support(A, -d);

		if (dot(a, d) < 0.0f) {
			return false;
		}

		sim2<float32> s = {a};

		d = -a;

		uint8 i = 0;

		while (i < GJK_2D_POINT_CONDITIONAL_ITERATION_LIMIT) {
			a = p - support(A, -d);

			if (dot(a, d) < 0.0f) {
				return false;
			}

			s += a;

			switch (s.count) {
				case 2:
					handleSimplex1(s, d);

					break;
				case 3:
					if (handleSimplex2(s, d)) {
						return true;
					}

					break;
				default:
					DEBUG_ASSERT(false);

					break;
			}

			i += 1;
		}

		ENGINE_WARN("GJK 2D point/conditional: Iteration limit reached.");

		return false;
	}
}