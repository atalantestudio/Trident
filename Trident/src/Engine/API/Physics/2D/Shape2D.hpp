// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/PhysicsMaterial.hpp"

namespace Trident {
	struct ShapeDescriptor2D {
		RigidBodyID2D rigidBodyId;
		PhysicsMaterial material;
		float32 density;
	};

	struct Shape2D {
		RigidBodyID2D rigidBodyId;
		PhysicsMaterial material;
		float32 density;
	};
}