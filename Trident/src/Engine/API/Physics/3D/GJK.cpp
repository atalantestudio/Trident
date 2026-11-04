// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/GJK.hpp"

namespace Trident {
	static void ClosestPointLineOrigin(Simplex3& s) {
		const vec3<float32> a = s[0].v;
		const vec3<float32> b = s[1].v;

		const vec3<float32> ab = b - a;

		const float32 uA = dot(ab, -a);

		if (uA <= 0.0f) {
			// Region A
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const float32 uB = dot(a - b, -b);

		if (uB <= 0.0f) {
			// Region B
			s[0] = s[1];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		// Region AB
		s[0].u = uB;
		s[1].u = uA;
		s.divisor = lengthSquared(ab);
		s.count = 2;
	}

	static void ClosestPointTriangleOrigin(Simplex3& s) {
		const vec3<float32> a = s[0].v;
		const vec3<float32> b = s[1].v;
		const vec3<float32> c = s[2].v;

		const vec3<float32> ao = -a;
		const vec3<float32> ab = b - a;
		const vec3<float32> ac = c - a;

		const float32 uAB = dot(ab, ao);
		const float32 uAC = dot(ac, ao);

		if (uAB <= 0.0f && uAC <= 0.0f) {
			// Region A
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec3<float32> bo = -b;
		const vec3<float32> bc = c - b;

		const float32 uBA = dot(a - b, bo);
		const float32 uBC = dot(bc, bo);

		if (uBA <= 0.0f && uBC <= 0.0f) {
			// Region B
			s[0] = s[1];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec3<float32> co = -c;
		const vec3<float32> ca = a - c;

		const float32 uCA = dot(ca, co);
		const float32 uCB = dot(b - c, co);

		if (uCA <= 0.0f && uCB <= 0.0f) {
			// Region C
			s[0] = s[2];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec3<float32> abac = cross(ab, ac);
		const float32 area = length(abac);
		const vec3<float32> normal = abac / area;

		const float32 uOAB = dot(cross(a, b), normal);

		if (uAB > 0.0f && uBA > 0.0f && uOAB <= 0.0f) {
			// Region AB
			s[0].u = uBA;
			s[1].u = uAB;
			s.divisor = lengthSquared(ab);
			s.count = 2;

			return;
		}

		const float32 uOBC = dot(cross(b, c), normal);

		if (uBC > 0.0f && uCB > 0.0f && uOBC <= 0.0f) {
			// Region BC
			s[0] = s[1];
			s[1] = s[2];
			s[0].u = uCB;
			s[1].u = uBC;
			s.divisor = lengthSquared(bc);
			s.count = 2;

			return;
		}

		const float32 uOCA = dot(cross(c, a), normal);

		if (uCA > 0.0f && uAC > 0.0f && uOCA <= 0.0f) {
			// Region CA
			s[1] = s[0];
			s[0] = s[2];
			s[0].u = uAC;
			s[1].u = uCA;
			s.divisor = lengthSquared(ca);
			s.count = 2;

			return;
		}

		// Region ABC
		s[0].u = uOBC;
		s[1].u = uOCA;
		s[2].u = uOAB;
		s.divisor = area;
		s.count = 3;
	}

	static vec3<float32> outwardsTriangleNormal(vec3<float32> ab, vec3<float32> ac, vec3<float32> ad) {
		vec3<float32> normal = cross(ab, ac);

		if (dot(normal, ad) > 0.0f) {
			normal *= -1.0f;
		}

		return normal;
	}

	static void ClosestPointTetrahedronOrigin(Simplex3& s) {
		const vec3<float32> a = s[0].v;
		const vec3<float32> b = s[1].v;
		const vec3<float32> c = s[2].v;
		const vec3<float32> d = s[3].v;

		const vec3<float32> ao = -a;
		const vec3<float32> ab = b - a;
		const vec3<float32> ac = c - a;
		const vec3<float32> ad = d - a;

		const float32 uAB = dot(ab, ao);
		const float32 uAC = dot(ac, ao);
		const float32 uAD = dot(ad, ao);

		if (uAB <= 0.0f && uAC <= 0.0f && uAD <= 0.0f) {
			// Region A
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec3<float32> bo = -b;
		const vec3<float32> ba = a - b;
		const vec3<float32> bc = c - b;
		const vec3<float32> bd = d - b;

		const float32 uBA = dot(ba, bo);
		const float32 uBC = dot(bc, bo);
		const float32 uBD = dot(bd, bo);

		if (uBA <= 0.0f && uBC <= 0.0f && uBD <= 0.0f) {
			// Region B
			s[0] = s[1];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec3<float32> co = -c;
		const vec3<float32> ca = a - c;
		const vec3<float32> cb = b - c;
		const vec3<float32> cd = d - c;

		const float32 uCA = dot(ca, co);
		const float32 uCB = dot(cb, co);
		const float32 uCD = dot(cd, co);

		if (uCA <= 0.0f && uCB <= 0.0f && uCD <= 0.0f) {
			// Region C
			s[0] = s[2];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec3<float32> _do = -d;
		const vec3<float32> da = a - d;
		const vec3<float32> db = b - d;
		const vec3<float32> dc = c - d;

		const float32 uDA = dot(da, _do);
		const float32 uDB = dot(db, _do);
		const float32 uDC = dot(dc, _do);

		if (uDA <= 0.0f && uDB <= 0.0f && uDC <= 0.0f) {
			// Region D
			s[0] = s[3];
			s[0].u = 1.0f;
			s.divisor = 1.0f;
			s.count = 1;

			return;
		}

		const vec3<float32> normalABC = outwardsTriangleNormal(ab, ac, ad);
		const vec3<float32> normalADB = outwardsTriangleNormal(ad, ab, ac);

		if (uAB > 0.0f && uBA > 0.0f && dot(cross(normalABC, ab), ao) > 0.0f && dot(cross(normalADB, ab), ao) > 0.0f) {
			// Region AB
			s[0].u = uBA;
			s[1].u = uAB;
			s.divisor = lengthSquared(ab);
			s.count = 2;

			return;
		}

		const vec3<float32> normalACD = outwardsTriangleNormal(ac, ad, ab);

		if (uAC > 0.0f && uCA > 0.0f && dot(cross(normalABC, ac), ao) > 0.0f && dot(cross(normalACD, ac), ao) > 0.0f) {
			// Region AC
			s[1] = s[2];
			s[0].u = uCA;
			s[1].u = uAC;
			s.divisor = lengthSquared(ac);
			s.count = 2;

			return;
		}

		if (uAD > 0.0f && uDA > 0.0f && dot(cross(normalADB, ad), ao) > 0.0f && dot(cross(normalACD, ad), ao) > 0.0f) {
			// Region AD
			s[1] = s[3];
			s[0].u = uDA;
			s[1].u = uAD;
			s.divisor = lengthSquared(ad);
			s.count = 2;

			return;
		}

		const vec3<float32> normalBCD = outwardsTriangleNormal(bc, bd, ba);

		if (uBC > 0.0f && uCB > 0.0f && dot(cross(normalABC, bc), bo) > 0.0f && dot(cross(normalBCD, bc), bo) > 0.0f) {
			// Region BC
			s[0] = s[1];
			s[1] = s[2];
			s[0].u = uCB;
			s[1].u = uBC;
			s.divisor = lengthSquared(bc);
			s.count = 2;

			return;
		}

		if (uBD > 0.0f && uDB > 0.0f && dot(cross(normalADB, bd), bo) > 0.0f && dot(cross(normalBCD, bd), bo) > 0.0f) {
			// Region BD
			s[0] = s[1];
			s[1] = s[3];
			s[0].u = uDB;
			s[1].u = uBD;
			s.divisor = lengthSquared(bd);
			s.count = 2;

			return;
		}

		if (uCD > 0.0f && uDC > 0.0f && dot(cross(normalACD, cd), co) > 0.0f && dot(cross(normalBCD, cd), co) > 0.0f) {
			// Region CD
			s[0] = s[2];
			s[1] = s[3];
			s[0].u = uDC;
			s[1].u = uCD;
			s.divisor = lengthSquared(cd);
			s.count = 2;

			return;
		}

		if (dot(normalABC, ao) > 0.0f) {
			const vec3<float32> abac = cross(ab, ac);

			// Region ABC
			s[0].u = dot(abac, cross(b, c));
			s[1].u = dot(abac, cross(c, a));
			s[2].u = dot(abac, cross(a, b));
			s.divisor = lengthSquared(normalABC);
			s.count = 3;

			return;
		}

		if (dot(normalACD, ao) > 0.0f) {
			const vec3<float32> acad = cross(ac, ad);

			// Region ACD
			s[1] = s[2];
			s[2] = s[3];
			s[0].u = dot(acad, cross(c, d));
			s[1].u = dot(acad, cross(d, a));
			s[2].u = dot(acad, cross(a, c));
			s.divisor = lengthSquared(normalACD);
			s.count = 3;

			return;
		}

		if (dot(normalADB, ao) > 0.0f) {
			const vec3<float32> adab = cross(ad, ab);

			// Region ADB
			s[2] = s[1];
			s[1] = s[3];
			s[0].u = dot(adab, cross(d, b));
			s[1].u = dot(adab, cross(b, a));
			s[2].u = dot(adab, cross(a, d));
			s.divisor = lengthSquared(normalADB);
			s.count = 3;

			return;
		}

		if (dot(normalBCD, bo) > 0.0f) {
			const vec3<float32> bcbd = cross(bc, bd);

			// Region BCD
			s[0] = s[1];
			s[1] = s[2];
			s[2] = s[3];
			s[0].u = dot(bcbd, cross(c, d));
			s[1].u = dot(bcbd, cross(d, b));
			s[2].u = dot(bcbd, cross(b, c));
			s.divisor = lengthSquared(normalBCD);
			s.count = 3;

			return;
		}

		// Region ABCD
		s[0].u = dot(b, cross(c, d));
		s[1].u = dot(a, cross(d, c));
		s[2].u = dot(a, cross(b, d));
		s[3].u = dot(a, cross(c, b));
		s.divisor = s[0].u + s[1].u + s[2].u + s[3].u;
		s.count = 4;
	}

	GJK3DOutput GJKPointPolygon3D(const RigidBody3DComponent& A, const RigidBody3DComponent& B, const Hull3D& hullA, const Hull3D& hullB) {
		static constexpr uint32 GJK_3D_ITERATION_LIMIT = 16;

		DEBUG_ASSERT(hullA.type == Hull3DType::SPHERE);
		DEBUG_ASSERT(hullB.type == Hull3DType::POLYGON);

		const quat<float32> RB = inverse(B.orientation);

		vec3<float32> d = B.position - A.position;

		Simplex3Vertex vertex;
		vertex.iA = 0;
		vertex.iB = hullB.supportIndex(d * RB);
		vertex.vA = A.position;
		vertex.vB = hullB.vertices[vertex.iB] * B.world;
		vertex.v = vertex.vB - vertex.vA;

		Simplex3 s = {vertex};
		uint32 saveB[4];
		uint8 i = 0;

		while (i < GJK_3D_ITERATION_LIMIT) {
			uint8 index = 0;

			while (index < s.count) {
				saveB[index] = s[index].iB;
				index += 1;
			}

			const uint8 countCopy = s.count;

			switch (s.count) {
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
				case 4: {
					ClosestPointTetrahedronOrigin(s);

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}

			if (s.count == 4) {
				break;
			}

			const vec3<float32> ao = -s[0].v;

			switch (s.count) {
				case 1: {
					d = ao;

					break;
				}
				case 2: {
					const vec3<float32> ab = s[1].v + ao;

					d = cross(cross(ab, ao), ab);

					break;
				}
				case 3: {
					const vec3<float32> ab = s[1].v + ao;
					const vec3<float32> ac = s[2].v + ao;

					d = cross(ab, ac);

					if (dot(d, ao) <= 0.0f) {
						d *= -1.0f;
					}

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}

			if (lengthSquared(d) < epsilon<float32>()) {
				ENGINE_WARNING("GJK 3D: Vertex, edge, face or tetrahedron overlap.");

				break;
			}

			Simplex3Vertex vertex;
			vertex.iA = 0;
			vertex.iB = hullB.supportIndex(d * RB);

			bool duplicateIndex = false;
			uint8 j = 0;

			while (j < countCopy) {
				if (vertex.iB == saveB[j]) {
					duplicateIndex = true;

					break;
				}

				j += 1;
			}

			if (duplicateIndex) {
				break;
			}

			vertex.vA = A.position;
			vertex.vB = hullB.vertices[vertex.iB] * B.world;
			vertex.v = vertex.vB - vertex.vA;

			s[s.count] = vertex;
			s.count += 1;

			#if defined(BUILD_MODE_DEBUG)
				if (i == GJK_3D_ITERATION_LIMIT - 1) {
					ENGINE_WARNING("GJK 3D: Iteration limit reached.");
				}
			#endif

			i += 1;
		}

		GJK3DOutput output;
		output.simplex = s;
		output.closestA = A.position;

		switch (s.count) {
			case 1: {
				output.closestB = s[0].vB;

				break;
			}
			case 2: {
				output.closestB = (s[0].vB * s[0].u + s[1].vB * s[1].u) / s.divisor;

				break;
			}
			case 3: {
				output.closestB = (s[0].vB * s[0].u + s[1].vB * s[1].u + s[2].vB * s[2].u) / s.divisor;

				break;
			}
			case 4: {
				output.closestB = (s[0].vB * s[0].u + s[1].vB * s[1].u + s[2].vB * s[2].u + s[3].vB * s[3].u) / s.divisor;

				break;
			}
			default: {
				DEBUG_ASSERT(false);

				break;
			}
		}

		output.distance = distance(output.closestA, output.closestB);

		return output;
	}
}