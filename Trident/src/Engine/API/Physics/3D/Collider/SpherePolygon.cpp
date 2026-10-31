// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/3D/Collider.hpp"

namespace Trident {
	bool SpherePolygon(const RigidBodyPair3D& pair, ContactManifold3D* contactManifold) {
		DEBUG_ASSERT(pair.hullA->type == Hull3DType::SPHERE);
		DEBUG_ASSERT(pair.hullB->type == Hull3DType::POLYGON);

		// TODO(physics/3d/collider/sphere-polygon)
		ENGINE_ERROR_UNIMPLEMENTED();

		return false;
	}
}