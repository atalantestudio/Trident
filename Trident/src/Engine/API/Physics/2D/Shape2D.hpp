// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/ShapeType.hpp"
#include "Engine/API/Physics/PhysicsMaterial.hpp"

namespace Trident {
	struct CircleShape2D {
		// In local space.
		vec2<float32> centroid;

		float32 radius;
	};

	struct ShapeDescriptor2D {
		ShapeType2D type;
		RigidBodyID2D rigidBodyId;
		PhysicsMaterial material;
		float32 density;

		union {
			CircleShape2D circle;
		};
	};

	struct Shape2D {
		Shape2D() {}
		~Shape2D() {}

		ShapeType2D type;
		RigidBodyID2D rigidBodyId;
		PhysicsMaterial material;
		float32 density;

		union {
			CircleShape2D circle;
		};
	};
}