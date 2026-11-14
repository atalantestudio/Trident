// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Shape/Shape.hpp"

namespace Trident {
	uint32 indexSupport(const Shape2D& shape, vec2<float32> d) {
		DEBUG_ASSERT(shape.type == ShapeType2D::CONVEX_HULL);

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

	vec2<float32> vertexSupport(const Shape2D& shape, vec2<float32> d) {
		switch (shape.type) {
			case ShapeType2D::CIRCLE: {
				return normalized(d) * shape.circle.radius;
			}
			case ShapeType2D::CONVEX_HULL: {
				return shape.hull->getVertices()[indexSupport(shape, d)];
			}
			default: {
				DEBUG_ASSERT(false);

				return {0.0f, 0.0f};
			}
		}
	}
}