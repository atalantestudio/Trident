// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/Shape/Hull/HullHeader.hpp"
#include "Engine/API/Physics/2D/Shape/ShapeType.hpp"
#include "Engine/API/Physics/Material.hpp"

namespace Trident {
	struct ShapeDescriptor2D {
		ShapeType2D type;
		RigidBodyID2D rigidBodyId;
		PhysicsMaterial material;
		float32 density;
	};

	struct CircleShape2D {
		// TODO(base/include): float32 isn't recognized when the struct is in its own file.
		float32 radius;
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
			const HullHeader2D* hull;
		};
	};

	/// Returns the index of the farthest vertex along `d`.
	uint32 indexSupport(
		/// The shape to evaluate.
		/// Must have the CONVEX_HULL type.
		const Shape2D& shape,

		/// The direction vector.
		/// Can be unnormalized.
		vec2<float32> d
	);

	/// Returns the farthest vertex along `d` in the local space of the shape.
	vec2<float32> vertexSupport(
		/// The shape to evaluate.
		const Shape2D& shape,

		/// The direction vector.
		/// Can be unnormalized.
		vec2<float32> d
	);
}