// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/CircleShape.hpp"
#include "Engine/API/Physics/2D/Hull/HullHeader.hpp"
#include "Engine/API/Physics/2D/ShapeType.hpp"
#include "Engine/API/Physics/PhysicsMaterial.hpp"

namespace Trident {
	struct ShapeDescriptor2D {
		ShapeType2D type;
		RigidBodyID2D rigidBodyId;
		PhysicsMaterial material;
		float32 density;
	};

	struct Shape2D {
		Shape2D() {}
		~Shape2D() {}

		ShapeType2D type;
		RigidBodyID2D rigidBodyId;
		PhysicsMaterial material;
		float32 density;
		uint32 userDataIndex;

		union {
			CircleShape2D circle;
			const HullHeader2D* hull;
		};
	};
}