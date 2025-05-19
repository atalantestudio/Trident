#pragma once

#include "Engine/API/Physics/GJK2D.hpp"

namespace ProjectA {
	static constexpr uint8 GJK_2D_POINT_ITERATION_LIMIT = 8;

	// Gilbert-Johnson-Keerthi object vs. point algorithm implementation in 2D.
	// References:
	// 
	// - Casey Muratori - Implementing GJK (2006)
	//   https://caseymuratori.com/blog_0003
	// - Erin Catto - Computing Distance using GJK - GDC 2010
	//   https://box2d.org/files/ErinCatto_GJK_GDC2010.pdf
	// - William Bittle - GJK, Distance, Closest Points
	//   https://dyn4j.org/2010/04/gjk-distance-closest-points
	// - Christer Ericson - Real-Time Collision Detection
	//   https://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
	// 
	inline GJK2DResult GJK2DPoint(const Object2D& A, const vec2<float32>& p) {
		DEBUG_ASSERT(A.getGeometry()->getVertexCount() < std::numeric_limits<uint8>::max());

		vec2<float32> d;

		Simplex2Vertex vertex;
		vertex.iA = 0;
		vertex.iB = 0;
		vertex.vA = transformSupport(A, vertex.iA);
		vertex.vB = p;
		vertex.v = vertex.vB - vertex.vA;

		Simplex2 s = {vertex};

		uint8 i = 0;

		while (i < GJK_2D_POINT_ITERATION_LIMIT) {
			const uint8 previousCount = s.count;

			fchain<uint32> saveA(s.count);
			fchain<uint32> saveB(s.count);

			for (uint8 j = 0; j < s.count; j += 1) {
				saveA[j] = s[j].iA;
				saveB[j] = s[j].iB;
			}

			const uint8 countCopy = s.count;

			switch (s.count) {
				case 1:
					break;
				case 2:
					ClosestPointLineOrigin(s);

					break;
				case 3:
					ClosestPointTriangleOrigin(s);

					break;
				default:
					DEBUG_ASSERT(false);

					break;
			}

			if (s.count == 3) {
				break;
			}

			switch (s.count) {
				case 1:
					d = -s[0].v;

					break;
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
				default:
					DEBUG_ASSERT(false);

					break;
			}

			if (lengthSq(d) < lyah::epsilon<float32>() * lyah::epsilon<float32>()) {
				ENGINE_WARN("GJK 2D: Vertex, edge or face overlap.");

				break;
			}

			Simplex2Vertex vertex;
			vertex.iA = supportIndex(A, -d);
			vertex.iB = 0;

			bool duplicate = false;

			for (uint8 j = 0; j < countCopy; j += 1) {
				if (vertex.iA == saveA[j] && vertex.iB == saveB[j]) {
					duplicate = true;

					break;
				}
			}

			if (duplicate) {
				break;
			}

			vertex.vA = transformSupport(A, vertex.iA);
			vertex.vB = p;
			vertex.v = vertex.vB - vertex.vA;

			s += vertex;

			#ifdef BUILD_MODE_DEBUG
				if (i == GJK_2D_ITERATION_LIMIT - 1) {
					ENGINE_WARN("GJK 2D: Iteration limit reached.");
				}
			#endif

			i += 1;
		}

		GJK2DResult result;

		const float32 div = lyah::rcp(s.divisor);

		switch (s.count) {
			case 1:
				result.closestA = s[0].vA;
				result.closestB = s[0].vB;

				break;
			case 2:
				result.closestA = (s[0].vA * s[0].u + s[1].vA * s[1].u) * div;
				result.closestB = (s[0].vB * s[0].u + s[1].vB * s[1].u) * div;

				break;
			case 3:
				result.closestA = (s[0].vA * s[0].u + s[1].vA * s[1].u + s[2].vA * s[2].u) * div;
				result.closestB = result.closestA;

				break;
			default:
				DEBUG_ASSERT(false);

				break;
		}

		result.distance = distance(result.closestA, result.closestB);

		return result;
	}
}