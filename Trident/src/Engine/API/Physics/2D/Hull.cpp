// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static uint32 getUserIndex() {
		static uint32 userIndex = 1;

		const uint32 index = userIndex;

		userIndex += 1;

		return index;
	}

	BoxHull2D createBoxHull(vec2<float32> halfSize) {
		const uint32 userIndex = getUserIndex();

		const float32 hx = halfSize.x;
		const float32 hy = halfSize.y;

		// const float32 x = hx * 2.0f;
		// const float32 y = hy * 2.0f;

		BoxHull2D boxHull;
		boxHull.header.centroid = {0.0f, 0.0f};
		// boxHull.header.area = x * y;
		// boxHull.header.radius = length(halfSize);
		// boxHull.header.inertia = boxHull.header.area * (x * x + y * y) / 12.0f;
		boxHull.header.vertexCount = 4;
		boxHull.header.vertexOffset = offsetof(BoxHull2D, vertices);
		boxHull.header.userIndex = userIndex;
		boxHull.vertices[0] = {-hx,  hy};
		boxHull.vertices[1] = { hx,  hy};
		boxHull.vertices[2] = { hx, -hy};
		boxHull.vertices[3] = {-hx, -hy};

		return boxHull;
	}

	BoxHull2D createHull(view<vec2<float32>> vertices) {
		DEBUG_ASSERT(vertices.count() == 3 || vertices.count() == 4);

		const uint32 userIndex = getUserIndex();
		const uint8 vertexCount = vertices.count();

		vec2<float32> centroid = {0.0f, 0.0f};
		float32 radiusSquared = 0.0f;
		uint8 vertexIndex = 0;

		while (vertexIndex < vertexCount) {
			const vec2<float32> vertex = vertices[vertexIndex];

			centroid += vertex;

			vertexIndex += 1;
		}

		centroid /= static_cast<float32>(vertexCount);

		BoxHull2D hull;
		hull.header.centroid = {0.0f, 0.0f};

		vertexIndex = 0;

		while (vertexIndex < vertexCount) {
			const vec2<float32> vertex = vertices[vertexIndex] - centroid;

			hull.vertices[vertexIndex] = vertex;

			radiusSquared = max(radiusSquared, lengthSquared(vertex));

			vertexIndex += 1;
		}

		// hull.header.area = 0.0f;
		// hull.header.radius = sqrt(radiusSquared);
		// hull.header.inertia = 0.0f;
		hull.header.vertexCount = vertexCount;
		hull.header.vertexOffset = offsetof(BoxHull2D, vertices);
		hull.header.userIndex = userIndex;

		return hull;
	}
}