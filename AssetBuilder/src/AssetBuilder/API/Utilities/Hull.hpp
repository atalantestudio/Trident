// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	template<>
	Hull2D createHull<Hull2DType::CIRCLE>(const Geometry2D& geometry, std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer) {
		const range<uint32, uint32> vertexPositionComponentRange = geometry.getVertexPositionComponentRange();
		const view<vec2<float32>> vertices(pointer_cast<const vec2<float32>*>(&vertexComponentBuffer[vertexPositionComponentRange.offset]), vertexPositionComponentRange.count / 2);

		float32 radius = 0.0_m;

		for (const vec2<float32> vertex : vertices) {
			radius = max(radius, lengthSquared(-vertex));
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

		vec2<float32>* vertices = pointer_cast<vec2<float32>*>(&vertexComponentBuffer[vertexPositionComponentRange.offset]);

		const uint32 vertexCount = vertexPositionComponentRange.count / 2;
		const uint32 lineIndexCount = vertexPositionComponentRange.count / 2 + 1;

		indexBuffer.resize(indexBuffer.size() + lineIndexCount);

		uint32 lineIndexIndex = 0;

		while (lineIndexIndex < lineIndexCount - 1) {
			indexBuffer[indexOffset + lineIndexIndex] = lineIndexIndex;

			lineIndexIndex += 1;
		}

		indexBuffer[indexOffset + lineIndexIndex] = 0;

		vec2<float32> centroid = {0.0_m, 0.0_m};
		uint32 vertexIndex = 0;

		while (vertexIndex < vertexCount) {
			centroid += vertices[vertexIndex];

			vertexIndex += 1;
		}

		centroid /= static_cast<float32>(vertexCount);

		float32 radius = 0.0_m;

		vertexIndex = 0;

		while (vertexIndex < vertexCount) {
			radius = max(radius, distanceSquared(vertices[vertexIndex], centroid));

			vertexIndex += 1;
		}

		radius = sqrt(radius);

		return {
			.type = Hull2DType::POLYGON,
			.centroid = centroid,
			.radius = radius,
			.vertices = view<vec2<float32>>(vertices, vertexCount),
			.lineIndexRange = {indexOffset, lineIndexCount},
			.edges = {},
		};
	}

	template<>
	Hull3D createHull<Hull3DType::POLYGON>(view<BuildVertex3D> vertices) {
		vec3<float32> centroid = {0.0f, 0.0f, 0.0f};
		float32 radius = 0.0f;

		for (const BuildVertex3D& vertex : vertices) {
			centroid += vertex.position;
		}

		centroid /= static_cast<float32>(vertices.count());

		for (const BuildVertex3D& vertex : vertices) {
			radius = max(radius, distanceSquared(vertex.position, centroid));
		}

		radius = sqrt(radius);

		return {
			.centroid = centroid,
			.radius = radius,
			.vertices = {},
			.edges = {},
			.faces = {},
			.lineIndexRange = {},
			.type = Hull3DType::POLYGON,
		};
	}
}