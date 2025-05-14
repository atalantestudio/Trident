#pragma once

#include "Engine/API/Object/Object2D.hpp"
#include "Engine/API/Physics/support.hpp"

namespace ProjectA {
	static constexpr uint8 GJK_CONDITIONAL_ITERATION_LIMIT = 8;
	static constexpr uint8 GJK_ITERATION_LIMIT = 8;

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

	// TODO: GJK 2D Point Conditional

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
		// TODO: Normalize or not?
		vec2<float32> d = B.position - A.position;

		// TODO: Does this case always end up in an intersection?
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

		for (uint8 iteration = 0; iteration < GJK_CONDITIONAL_ITERATION_LIMIT; iteration += 1) {
			// Find a support point on the Minkowski difference of A and B.
			a = support(B, d) - support(A, -d);

			// Check if a passes the origin.
			if (dot(a, d) < 0.0f) {
				return false;
			}

			s += a;

			DEBUG_ASSERT(s.count == 2 || s.count == 3);

			if (s.count == 2) {
				handle1Simplex(s, d);

				continue;
			}

			if (handle2Simplex(s, d)) {
				return true;
			}
		}

		ENGINE_WARN("Max iteration limit reached.");

		return false;
	}

	struct SimplexVertex {
		// Support point on A in world space.
		vec2<float32> vA;

		// Support point on A in world space.
		vec2<float32> vB;

		// vB - vA.
		vec2<float32> v;

		// Vertex index on A.
		uint32 iA = 0;

		// Vertex index on B.
		uint32 iB = 0;

		// Barycentric coordinate of the closest point.
		float32 u = 0.0f;
	};

	struct Simplex {
		SimplexVertex vertices[3];
		float32 divisor = 1.0f;
		uint8 count : 2;

		Simplex() : count(0) {}

		const SimplexVertex& operator [](std::size_t index) const {
			DEBUG_ASSERT(index < 3);

			return vertices[index];
		}

		SimplexVertex& operator [](std::size_t index) {
			DEBUG_ASSERT(index < 3);

			return vertices[index];
		}
	};

	struct GJK2DResult {
		const Object2D* A;
		const Object2D* B;

		Simplex simplex;

		// Closest point on A.
		vec2<float32> closestA;

		// Closest point on B.
		vec2<float32> closestB;

		// Minimum distance between A and B.
		float32 distance;
	};

	// Calculates the closest point to the origin on a line formed by s.
	static void ClosestPointLineOrigin(Simplex& s) {
		const vec2<float32>& a = s[0].v;
		const vec2<float32>& b = s[1].v;
		const vec2<float32> ab = b - a;

		const float32 uAB = dot(-a, ab);

		if (uAB < 0.0f) {
			// In region A.
			s[0].u = uAB;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const float32 vAB = dot(-b, a - b);

		if (vAB < 0.0f) {
			// In region B.
			s[0] = s[1];
			s[0].u = vAB;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		// In region AB.
		s[0].u = vAB;
		s[1].u = uAB;
		s.divisor = lengthSq(ab);
		s.count = 2;
	}

	// Calculates the closest point to the origin on a triangle formed by s.
	static void ClosestPointTriangleOrigin(Simplex& s) {
		const vec2<float32>& a = s[0].v;
		const vec2<float32>& b = s[1].v;
		const vec2<float32>& c = s[2].v;
		const vec2<float32> ab = b - a;
		const vec2<float32> ac = c - a;

		const float32 uAB = dot(-a, ab);
		const float32 vCA = dot(-a, ac);

		if (vCA <= 0.0f && uAB <= 0.0f) {
			// In region A.
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec2<float32> bc = c - b;

		const float32 vAB = dot(-b, a - b);
		const float32 uBC = dot(-b, bc);

		if (vAB <= 0.0f && uBC <= 0.0f) {
			// In region B.
			s[0] = s[1];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec2<float32> ca = a - c;

		const float32 vBC = dot(-c, b - c);
		const float32 uCA = dot(-c, ca);

		if (vBC <= 0.0f && uCA <= 0.0f) {
			// In region C.
			s[0] = s[2];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		// Compute signed parallelogram area.
		const float32 S = area(ab, ac);

		// Compute triangle barycentric coordinates (pre-division).
		const float32 uABC = area(b, c);
		const float32 vABC = area(c, a);
		const float32 wABC = area(a, b);

		if (uAB > 0.0f && vAB > 0.0f && wABC * S <= 0.0f) {
			// In region AB.
			s[0].u = vAB;
			s[1].u = uAB;
			s.divisor = lengthSq(ab);
			s.count = 2;

			return;
		}

		if (uBC > 0.0f && vBC > 0.0f && uABC * S <= 0.0f) {
			// In region BC.
			s[0] = s[1];
			s[1] = s[2];
			s[0].u = vBC;
			s[1].u = uBC;
			s.divisor = lengthSq(bc);
			s.count = 2;

			return;
		}

		if (uCA > 0.0f && vCA > 0.0f && vABC * S <= 0.0f) {
			// In region CA.
			s[1] = s[0];
			s[0] = s[2];
			s[0].u = vCA;
			s[1].u = uCA;
			s.divisor = lengthSq(ca);
			s.count = 2;

			return;
		}

		// Without CCD, this throws when the shapes separate after an intersection.
		// With CCD, this is never reached since no intersection should ever occur.
		// DEBUG_ASSERT(uABC > 0.0f && vABC > 0.0f && wABC > 0.0f);

		// In region ABC.
		s[0].u = uABC;
		s[1].u = vABC;
		s[2].u = wABC;
		s.divisor = S;
		s.count = 3;
	}

	inline GJK2DResult GJK2D(const Object2D& A, const Object2D& B) {
		DEBUG_ASSERT(A.getGeometry().vertices.size() < std::numeric_limits<uint8>::max());
		DEBUG_ASSERT(B.getGeometry().vertices.size() < std::numeric_limits<uint8>::max());

		GJK2DResult result{};
		result.A = &A;
		result.B = &B;

		Simplex& s = result.simplex;

		SimplexVertex& vertex = s[0];
		vertex.iA = 0;
		vertex.iB = 0;
		vertex.vA = transformSupport(A, vertex.iA);
		vertex.vB = transformSupport(B, vertex.iB);
		vertex.v = vertex.vB - vertex.vA;
		vertex.u = 1.0f;

 		s.count = 1;

		//vec2<float32> d = B.position - A.position;
		vec2<float32> d;

		for (uint8 i = 0; i < GJK_ITERATION_LIMIT; i += 1) {
			// debug
			for (uint8 k = 0; k < s.count; k += 1) {
				if (lengthSq(s[k].v) == 0.0f) {
					BREAKPOINT();
				}
			}

			const uint8 previousCount = s.count;
			fchain<uint32> saveA(previousCount);
			fchain<uint32> saveB(previousCount);

			for (uint8 j = 0; j < previousCount; j += 1) {
				saveA[j] = s[j].iA;
				saveB[j] = s[j].iB;
			}

			switch (previousCount) {
				case 1: {
					break;
				}
				case 2: {
					ClosestPointLineOrigin(s);

					break;
				}
				case 3: {
					ClosestPointTriangleOrigin(s);

					break;
				}
				default: {
					DEBUG_ASSERT(false, "Invalid simplex dimension.");

					break;
				}
			}

			if (s.count == 3) {
				break;
			}

			switch (s.count) {
				case 1: {
					d = -s[0].v;

					break;
				}
				case 2: {
					const vec2<float32> ab = s[1].v - s[0].v;
					const float32 sign = area(ab, -s[0].v);

					if (sign > 0.0f) {
						d = {-ab[1], ab[0]};
					}
					else {
						d = {ab[1], -ab[0]};
					}

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}

			if (lengthSq(d) < lyah::epsilon<float32>() * lyah::epsilon<float32>()) {
				ENGINE_WARN("GJK: Search direction is 0 (vertex overlap).");

				break;
			}

			SimplexVertex& vertex = s[s.count];
			vertex.iA = supportIndex(A, -d);
			vertex.iB = supportIndex(B, d);

			bool duplicate = false;

			for (uint8 j = 0; j < previousCount; j += 1) {
				if (vertex.iA == saveA[j] && vertex.iB == saveB[j]) {
					duplicate = true;

					break;
				}
			}

			if (duplicate) {
				break;
			}

			vertex.vA = transformSupport(A, vertex.iA);
			vertex.vB = transformSupport(B, vertex.iB);
			vertex.v = vertex.vB - vertex.vA;

			s.count += 1;

			#ifdef BUILD_MODE_DEBUG
				if (i == GJK_ITERATION_LIMIT - 1) {
					//ENGINE_WARN("GJK 2D: Iteration limit reached.");
				}
			#endif
		}

		const float32 div = lyah::rcp(s.divisor);

		// Find the closest points on both objects.
		switch (s.count) {
			case 1: {
				result.closestA = s[0].vA;
				result.closestB = s[0].vB;

				break;
			}
			case 2: {
				result.closestA =
					s[0].vA * s[0].u * div + 
					s[1].vA * s[1].u * div;
				result.closestB =
					s[0].vB * s[0].u * div +
					s[1].vB * s[1].u * div;

				break;
			}
			case 3: {
				result.closestA =
					s[0].vA * s[0].u * div +
					s[1].vA * s[1].u * div +
					s[2].vA * s[2].u * div;
				result.closestB = result.closestA;

				break;
			}
		}

		result.distance = distance(result.closestA, result.closestB);

		return result;
	}
}