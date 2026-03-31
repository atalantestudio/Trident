// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"

namespace Trident {
	typedef bool (*Collider2D)(const RigidBody2D& A, const RigidBody2D& B, const Hull2D& hullA, const Hull2D& hullB, ContactManifold2D* contactManifold);
}