// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/3D/ContactManifold3D.hpp"
#include "Engine/API/Physics/3D/RigidBodyPair.hpp"

namespace Trident {
	typedef bool (*Collider3D)(const RigidBodyPair3D& pair, ContactManifold3D* contactManifold);

	bool PolygonPolygon(const RigidBodyPair3D& pair, ContactManifold3D* contactManifold);
}