// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	template<>
	Hull2D createHull<Hull2DType::CIRCLE>(const Geometry2D& geometry, std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer) {
		const range<uint32, uint32> vertexPositionComponentRange = geometry.getVertexPositionComponentRange();
		const view<svec2<float32>> vertices(pointer_cast<const svec2<float32>*>(&vertexComponentBuffer[vertexPositionComponentRange.offset]), vertexPositionComponentRange.count / 2);

		float32 radius = 0.0_m;

		for (const svec2<float32> vertex : vertices) {
			radius = max(radius, lengthSquared(-vec2<float32>(vertex)));
		}

		radius = sqrt(radius);

		return {
			.type = Hull2DType::CIRCLE,
			.centroid = {0.0_m, 0.0_m},
			.radius = radius,
			.vertices = {},
			.lineIndexRange = {static_cast<uint32>(indexBuffer.size()), 0},
			.edges = {},
		};
	}

	template<>
	Hull2D createHull<Hull2DType::POINT>(const Geometry2D& geometry, std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer) {
		return Hull2D::point();
	}

	template<>
	Hull2D createHull<Hull2DType::POLYGON>(const Geometry2D& geometry, std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer) {
		const uint32 indexOffset = static_cast<uint32>(indexBuffer.size());
		const range<uint32, uint32> vertexPositionComponentRange = geometry.getVertexPositionComponentRange();
		const view<svec2<float32>> vertices(pointer_cast<const svec2<float32>*>(&vertexComponentBuffer[vertexPositionComponentRange.offset]), vertexPositionComponentRange.count / 2);
		const uint32 lineIndexCount = vertexPositionComponentRange.count / 2 + 1;

		indexBuffer.resize(indexBuffer.size() + lineIndexCount);

		uint32 lineIndexIndex = 0;

		while (lineIndexIndex < lineIndexCount - 1) {
			indexBuffer[indexOffset + lineIndexIndex] = lineIndexIndex;

			lineIndexIndex += 1;
		}

		indexBuffer[indexOffset + lineIndexIndex] = 0;

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

		return {
			.type = Hull2DType::POLYGON,
			.centroid = {centroid[0], centroid[1]},
			.radius = radius,
			.vertices = vertices,
			.lineIndexRange = {indexOffset, lineIndexCount},
			.edges = {},
		};
	}

	template<>
	Hull3D createHull<Hull3DType::POLYGON>(view<BuildVertex3D> vertices) {
		svec3<float32> centroid = {0.0f, 0.0f, 0.0f};
		float32 radius = 0.0f;

		for (const BuildVertex3D& vertex : vertices) {
			centroid += vertex.position;
		}

		centroid /= static_cast<float32>(vertices.count());

		for (const BuildVertex3D& vertex : vertices) {
			// TODO(lyah/scalar)
			radius = max(radius, distanceSquared(vec3<float32>(vertex.position), vec3<float32>(centroid)));
		}

		radius = sqrt(radius);

		return {
			.centroid = centroid,
			.radius = radius,
			.vertexPositions = {},
			.edges = {},
			.faces = {},
			.lineIndexRange = {},
			.type = Hull3DType::POLYGON,
		};
	}
}