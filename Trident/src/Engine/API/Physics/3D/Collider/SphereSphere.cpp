// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"

namespace Trident {
	bool SphereSphere(const RigidBodyPair3D& pair, ContactManifold3D* manifold) {
		static constexpr float32 COINCIDENT_CIRCLE_BIAS = 0.001f;

		DEBUG_ASSERT(pair.hullA->type == Hull3DType::SPHERE);
		DEBUG_ASSERT(pair.hullB->type == Hull3DType::SPHERE);

		const vec3<float32> a = pair.rigidBodyA->position;
		const vec3<float32> b = pair.rigidBodyB->position;
		const vec3<float32> ab = b - a;

		const float32 dd = lengthSquared(ab);

		const float32 rA = pair.hullA->radius * pair.rigidBodyA->scale;
		const float32 rB = pair.hullB->radius * pair.rigidBodyB->scale;
		const float32 r = rA + rB;

		if (dd > r * r) {
			return false;
		}

		// Degenerate case (coincident spheres).
		if (dd < COINCIDENT_CIRCLE_BIAS) {
			return false;
		}

		const float32 d = sqrt(dd);
		const vec3<float32> normal = ab / d;

		manifold->normal = normal;
		manifold->contactCount = 1;

		Contact3D& contact = manifold->contacts[0];
		contact.position = (a + normal * rA + b - normal * rB) * 0.5f;
		contact.separation = d - r;

		return true;
	}
}