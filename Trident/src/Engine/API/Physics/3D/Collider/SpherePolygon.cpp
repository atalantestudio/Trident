// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"
#include "Engine/API/Physics/3D/GJK.hpp"

namespace Trident {
	bool SpherePolygon(const RigidBodyPair3D& pair, ContactManifold3D* contactManifold) {
		DEBUG_ASSERT(pair.hullA->type == Hull3DType::SPHERE);
		DEBUG_ASSERT(pair.hullB->type == Hull3DType::POLYGON);

		const float32 r = pair.hullA->radius;
		const GJK3DOutput gjkOutput = GJKPointPolygon3D(*pair.rigidBodyA, *pair.rigidBodyB, *pair.hullA, *pair.hullB);

		ENGINE_DEBUG(gjkOutput.distance - r);

		if (gjkOutput.distance > r) {
			return false;
		}

		if (gjkOutput.distance > 0.0f) {
			contactManifold->normal = normalized(gjkOutput.closestB - pair.rigidBodyA->position);
			contactManifold->contactCount = 1;

			Contact3D& contact = contactManifold->contacts[0];
			contact.position = gjkOutput.closestB;
			contact.separation = gjkOutput.distance - r;

			return true;
		}

		// TODO(physics/3d/sat/sphere-polygon): Implement SAT variant.
		ENGINE_ERROR_UNIMPLEMENTED();

		return true;
	}
}