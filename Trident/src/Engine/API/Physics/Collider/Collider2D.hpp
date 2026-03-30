// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"

namespace Trident {
	typedef bool (*Collider2D)(const RigidBody2D& A, const RigidBody2D& B, const Geometry2D& geometryA, const Geometry2D& geometryB, ContactManifold2D* contactManifold);
}