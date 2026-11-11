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

		union {
			CircleShape2D circle;
			const HullHeader2D* hull;
		};
	};

	// Returns the index of the farthest vertex along `d` in the local space of the shape.
	inline uint32 indexSupport(const Shape2D& shape, vec2<float32> d) {
		const vec2<float32>* vertices = shape.hull->getVertices();

		float32 maxValue = dot(vertices[0], d);
		uint32 maxVertexIndex = 0;
		uint32 vertexIndex = 1;

		while (vertexIndex < shape.hull->vertexCount) {
			const float32 value = dot(vertices[vertexIndex], d);

			if (value > maxValue) {
				maxValue = value;
				maxVertexIndex = vertexIndex;
			}

			vertexIndex += 1;
		}

		return maxVertexIndex;
	}

	// Returns the farthest vertex along `d` in the local space of the shape.
	inline vec2<float32> vertexSupport(const Shape2D& shape, vec2<float32> d) {
		return shape.hull->getVertices()[indexSupport(shape, d)];
	}
}