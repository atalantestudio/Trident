#pragma once

#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Physics/Simplex.hpp"
#include "Engine/API/Physics/support.hpp"

namespace ProjectA {
	static constexpr uint8 GJK_2D_ITERATION_LIMIT = 8;

	// The result of GJK in 2D.
	struct GJK2DOutput {
		// Final simplex.
		Simplex2 simplex;

		// Closest point on A.
		vec2<float32> closestA;

		// Closest point on B.
		vec2<float32> closestB;

		// Minimum distance between A and B.
		// Calculated as the length of the difference between closestA and closestB.
		// A and B are intersecting if the distance is <= 0.
		float32 distance;

		// Last search direction used by GJK.
		vec2<float32> separatingAxis;

		// Closest feature on A. Can either be a vertex or an edge.
		Feature2D closestFeatureA;

		// Closest feature on B. Can either be a vertex or an edge.
		Feature2D closestFeatureB;
	};

	// Calculates the closest point to the origin on a line formed by the simplex s.
	static void ClosestPointLineOrigin(Simplex2& s) {
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

	// Calculates the closest point to the origin on a triangle formed by the simplex s.
	static void ClosestPointTriangleOrigin(Simplex2& s) {
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
		const float32 S = parallelogramArea(ab, ac);

		// Compute triangle barycentric coordinates (pre-division).
		const float32 uABC = parallelogramArea(b, c);
		const float32 vABC = parallelogramArea(c, a);
		const float32 wABC = parallelogramArea(a, b);

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

	// Gilbert-Johnson-Keerthi algorithm implementation in 2D.
	inline GJK2DOutput GJK2D(const RigidBody2D& A, const RigidBody2D& B) {
		DEBUG_ASSERT(A.geometry->getVertexCount() < max<uint8>());
		DEBUG_ASSERT(B.geometry->getVertexCount() < max<uint8>());

		vec2<float32> d = B.position - A.position;

		Simplex2Vertex vertex;
		vertex.iA = 0;
		vertex.iB = 0;
		vertex.vA = multiplyTransform(A.geometry->vertices[vertex.iA], A.transform);
		vertex.vB = multiplyTransform(B.geometry->vertices[vertex.iB], B.transform);
		vertex.v = vertex.vB - vertex.vA;

		Simplex2 s = {vertex};

		uint32 saveA[3];
		uint32 saveB[3];

		uint8 i = 0;

		while (i < GJK_2D_ITERATION_LIMIT) {
			const uint8 previousCount = s.count;

			for (uint8 index = 0; index < s.count; index += 1) {
				saveA[index] = s[index].iA;
				saveB[index] = s[index].iB;
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
					const float32 sign = parallelogramArea(ab, -s[0].v);

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

			if (lengthSq(d) < epsilon<float32>() * epsilon<float32>()) {
				ENGINE_WARN("GJK 2D: Vertex, edge or face overlap.");

				break;
			}

			Simplex2Vertex vertex;
			vertex.iA = supportIndex(A, -d);
			vertex.iB = supportIndex(B, d);

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

			vertex.vA = multiplyTransform(A.geometry->vertices[vertex.iA], A.transform);
			vertex.vB = multiplyTransform(B.geometry->vertices[vertex.iB], B.transform);
			vertex.v = vertex.vB - vertex.vA;

			s[s.count] = vertex;
			s.count += 1;

			#ifdef BUILD_MODE_DEBUG
				if (i == GJK_2D_ITERATION_LIMIT - 1) {
					ENGINE_WARN("GJK 2D: Iteration limit reached.");
				}
			#endif

			i += 1;
		}

		GJK2DOutput output;

		const float32 div = lyah::rcp(s.divisor);

		switch (s.count) {
			case 1:
				output.closestA = s[0].vA;
				output.closestB = s[0].vB;

				break;
			case 2:
				output.closestA = (s[0].vA * s[0].u + s[1].vA * s[1].u) * div;
				output.closestB = (s[0].vB * s[0].u + s[1].vB * s[1].u) * div;

				break;
			case 3:
				output.closestA = (s[0].vA * s[0].u + s[1].vA * s[1].u + s[2].vA * s[2].u) * div;
				output.closestB = output.closestA;

				break;
			default:
				DEBUG_ASSERT(false);

				break;
		}

		output.closestFeatureA.vertex = s[0].vA;
		output.closestFeatureA.setVertexCount(1);

		output.closestFeatureB.vertex = s[0].vB;
		output.closestFeatureB.setVertexCount(1);

		if (s.count == 2) {
			if (s[0].iA != s[1].iA) {
				output.closestFeatureA.edge[1] = s[1].vA;
				output.closestFeatureA.setVertexCount(2);
			}

			if (s[0].iB != s[1].iB) {
				output.closestFeatureB.edge[1] = s[1].vB;
				output.closestFeatureB.setVertexCount(2);
			}
		}

		output.distance = distance(output.closestA, output.closestB);
		output.separatingAxis = d;

		return output;
	}
}