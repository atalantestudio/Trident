// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"

namespace Trident {
	bool SphereSphere(const RigidBodyPair3D& pair, ContactManifold3D* contactManifold) {
		DEBUG_ASSERT(pair.hullA->type == Hull3DType::SPHERE);
		DEBUG_ASSERT(pair.hullB->type == Hull3DType::SPHERE);

		const vec3<float32> a = pair.rigidBodyA->position;
		const vec3<float32> b = pair.rigidBodyB->position;
		const vec3<float32> ab = b - a;

		const float32 rA = pair.hullA->radius * pair.rigidBodyA->scale;
		const float32 rB = pair.hullB->radius * pair.rigidBodyB->scale;
		const float32 r = rA + rB;

		const float32 dd = lengthSquared(ab);

		if (dd > r * r) {
			return false;
		}

		// Handle coincident spheres.
		if (abs(dd) < 0.0001f) {
			return false;
		}

		const float32 d = sqrt(dd);
		const vec3<float32> normal = ab / d;

		contactManifold->normal = normal;
		contactManifold->contactCount = 1;

		Contact3D& contact = contactManifold->contacts[0];
		contact.position = (a + normal * rA + b - normal * rB) * 0.5f;
		contact.separation = d - r;

		return true;
	}
}