// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	typedef bool (*Collider2D)(const RigidBody2DComponent& A, const RigidBody2DComponent& B, const Hull2D& hullA, const Hull2D& hullB, ContactManifold2D* contactManifold);
}