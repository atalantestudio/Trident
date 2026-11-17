// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/GJK/GJKConditional.hpp"

namespace Trident {
	static void handleSimplex1(sim2<float32>& s, vec2<float32>& d) {
		const vec2<float32> a = s[1];
		const vec2<float32> b = s[0];
		const vec2<float32> ab = b - a;
		const vec2<float32> ao = -a;

		d = triple(ab, ao, ab);
	}

	static bool handleSimplex2(sim2<float32>& s, vec2<float32>& d) {
		const vec2<float32> a = s[2];
		const vec2<float32> b = s[1];
		const vec2<float32> c = s[0];
		const vec2<float32> ab = b - a;
		const vec2<float32> ac = c - a;
		const vec2<float32> ao = -a;
		const vec2<float32> abPerp = triple(ac, ab, ab);
		const vec2<float32> acPerp = triple(ab, ac, ac);

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

	bool GJKConditional2D(const RigidBody2D& A, vec2<float32> p, const Shape2D& shapeA) {
		constexpr uint8 GJK_2D_CONDITIONAL_ITERATION_LIMIT = 8;

		const mat3x3<float32> worldA = A.getWorld();

		const mat2x2<float32> inverseRotationA = rotationAngle<mat2x2<float32>>(-A.rotation);

		vec2<float32> d = p - A.transform.position;

		if (lengthSquared(d) == 0.0f) {
			// Degenerate case: The objects have the same position.
			return true;
		}

		vec2<float32> a = vertexSupport(shapeA, d * inverseRotationA) * worldA - p;

		if (dot(a, d) < 0.0f) {
			return false;
		}

		sim2<float32> s = {a};

		d = -a;

		uint8 iteration = 0;

		while (iteration < GJK_2D_CONDITIONAL_ITERATION_LIMIT) {
			a = vertexSupport(shapeA, d * inverseRotationA) * worldA - p;

			if (dot(a, d) < 0.0f) {
				return false;
			}

			s[s.count] = a;
			s.count += 1;

			if (s.count == 2) {
				handleSimplex1(s, d);

				iteration += 1;

				continue;
			}

			if (handleSimplex2(s, d)) {
				return true;
			}

			iteration += 1;
		}

		ENGINE_WARNING("GJKConditional2D: Iteration limit reached.");

		return false;
	}
}