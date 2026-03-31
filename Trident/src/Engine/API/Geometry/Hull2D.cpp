// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Hull2D.hpp"

namespace Trident {
	Hull2D Hull2D::point() {
		return Hull2D({
			.type = Hull2DType::POINT,
			.centroid = {0.0_m, 0.0_m},
			.radius = 0.0_m,
			// .vertexOffset = 0,
			// .indexOffset = 0,
			// .triangleListIndexCount = 0,
			// .lineStripIndexCount = 0,
		});
	}

	template<>
	Hull2D Hull2D::fromGeometry<Hull2DType::CIRCLE>(const Geometry2D& geometry) {
		float32 radius = 0.0_m;

		for (const svec2<float32> vertex : geometry.getVertices()) {
			radius = max(radius, lengthSquared(-vec2<float32>(vertex)));
		}

		radius = sqrt(radius);

		return Hull2D({
			.type = Hull2DType::CIRCLE,
			.centroid = {0.0_m, 0.0_m},
			.radius = radius,
			.vertices = {},
			.fillIndices = {},
			.lineIndices = {},
		});
	}

	template<>
	Hull2D Hull2D::fromGeometry<Hull2DType::POINT>(const Geometry2D& geometry) {
		return Hull2D::point();
	}

	template<>
	Hull2D Hull2D::fromGeometry<Hull2DType::POLYGON>(const Geometry2D& geometry) {
		const view<svec2<float32>> vertices = geometry.getVertices();

		vec2<float32> centroid = {0.0_m, 0.0_m};
		float32 radius = 0.0_m;

		for (const svec2<float32> vertex : vertices) {
			centroid += vec2<float32>(vertex);
		}

		centroid /= static_cast<float32>(vertices.count());

		for (const svec2<float32> vertex : vertices) {
			radius = max(radius, distanceSquared(vec2<float32>(vertex), centroid));
		}

		radius = sqrt(radius);

		return Hull2D({
			.type = Hull2DType::POLYGON,
			.centroid = {centroid[0], centroid[1]},
			.radius = radius,
			.vertices = geometry.getVertices(),
			.fillIndices = geometry.getIndices(),
			.lineIndices = {},
		});
	}

	Hull2D::Hull2D(const Descriptor<Hull2D>& descriptor) :
		type(descriptor.type),
		centroid(descriptor.centroid),
		radius(descriptor.radius),
		vertices(descriptor.vertices),
		// fillIndices(descriptor.fillIndices),
		// lineIndices(descriptor.lineIndices),
		edges(descriptor.fillIndices.count() / 3 * 2)
	{
		uint32 edgeIndex = 0;

		while (edgeIndex < edges.count()) {
			edges[edgeIndex] = {edgeIndex, (edgeIndex + 1) % edges.count()};

			edgeIndex += 1;
		}
	}
}